# Data Development

## Runtime Databases

All paths are relative to the game runtime directory, not the repository root. `DatabaseCache` loads:

- `Data/Database/base.db`: UI, graphics, and settings data. It is skipped in headless mode.
- `Data/Database/data.db`: nations, units, buildings, resources, levels, technologies, and their relationships.
- `Data/map/maps.db`: map/mission metadata and the map-owned age catalog.
- `saves/<name>.db`: a scene save created by `SceneSaver`; `SceneLoader` receives the filename, including its `.db` suffix.

The loading implementation is `game/src/database/DatabaseCache.cpp`; save loading is `game/src/scene/load/SceneLoader.cpp`. Every floating-point value written by the current save format is stored as an integer multiplied by `config.precision` and divided by that precision during load; runtime DTOs remain floating-point values. The loader also reads legacy `REAL` cells as already-unscaled values.

The runtime data databases and save tables use different query contracts. `DatabaseCache` reads data tables with explicit column lists generated from the physical-column enums and decodes result columns by ordinal. Save writes use named-column `INSERT`s generated from the save enums, while save loads validate required column names and generate explicit `SELECT` lists from those enums. For saves, SQLite physical column order and extra columns do not matter, but expected column names, enum order, and bindings must stay aligned. For `DatabaseCache` tables, inserting, removing, or reordering a column still requires updating the matching enum and binding.

`DatabaseCache` reads `Data/Database/base.db`, but its current settings-write paths open `Data/base.db` and target `graphics_settings` while the loaded table is `graph_settings`. Treat graphics/settings persistence as a known limitation until those paths are corrected; do not rely on UI changes being written back to the loaded database.

The runtime `game/Data/Database/data.db` contains the technology tables and sample data. Each `technology` row stores a comma-separated `research_building` list of concrete building IDs, such as `5,2`; a technology can therefore be researched in any listed building. The selected building must be ready with an idle queue, and the technology must not already be researched by the player. `DatabaseCache` validates each listed building ID while loading. The value `none` means that the technology has no eligible research buildings. Technology levels contain only level-specific age gates, costs, and research time. Technology effects use a stable global `id`; `effect_order` is not part of the current schema. When active effects are combined, additive effects (including negative values) are applied before percentage effects, and effects within each operation are ordered by ascending ID. Runtime conversion to integer stat types happens after all applicable effects for that stat have been combined. Technology level display keys are derived at load time as `<technology.code>_<level>`, `<technology.code>_<level>_description`, and `<technology.code>_<level>.png`; they are not duplicated in `technology_level`. The runtime database is ignored by Git; keep the local database aligned with the code before starting the game. `DatabaseCache` treats a missing technology table as a required data error and stops startup rather than silently running without research.

## Age-Related Database Structure

World-age content is split by ownership. Static definitions are loaded from the installed databases; a save contains only the mutable age state for one match.

```text
Data/Database/data.db                 shared gameplay definitions
  unit_level(..., age_stage)          minimum logical stage for a unit level
  building_level(..., age_stage)      minimum logical stage for a building level

Data/map/maps.db                      map-owned progression catalog
  map(id, xmlName, name, age_ids)     ordered candidate age IDs for each map
  age(id, stage, name)                 age identity, stage, and localization key
  condition(id, metric, target)       reusable worker/army threshold
  age_condition(age_id, condition_id) age-to-condition join; all joins are ANDed
  age_transition(age_id, next_age_id) direct progression edges between ages

saves/<name>.db                       one scene and its continuation state
  config(..., map, total_ticks, ...)  selected map and simulation clock
  world_age(current_age,              selected age, entry tick, reached route
            age_started_tick, history)
```

The current checked-in age catalog uses logical stages plus explicit progression edges:

```text
stage 0: age 0  Age of Settlement
          /   \
stage 1: age 1  Age of Growth       (workers >= 10)
          |     age 2  Age of Mobilization (army >= 20)
stage 2: age 3  Age of Consolidation (workers >= 20)
          |     age 4  Age of Fortification (army >= 30)
stage 3: age 5  Age of Industry      (workers >= 40)
                age 6  Age of Conquest (army >= 70)
```

`map.age_ids` supplies the candidate order and deterministic tie-break. The controller only evaluates direct targets from `age_transition` that are listed by the selected map. There is no `world_age_node` or hold table in the active schema.

The checked-in map rows are `map 0 -> 0,1` and `map 1 -> 0,1,2,3,4,5,6`. The runtime `data.db` currently contains nine unit-level rows and 18 building-level rows at each of `age_stage` 0, 1, and 2. `age_stage` is a shared logical gate: a level is available when the selected map age reaches at least that stage, regardless of which candidate ID won within the stage. See [Map-Owned Age System](map-age-system.md) and [World Age Database Schema](world-age-database-schema.html) for the complete age table and save examples.

## ID And Relationship Invariants

ID-keyed entity caches use database ID as their vector index. `setEntity()` resizes a vector and stores each entity at `array[id]`; accessors such as `getUnit(id)` directly index that vector. A reference must therefore name an existing row with the intended ID. Never rely on SQL result position as identity. HUD-size and HUD-variable lists are exceptions: they are append-only display lists rather than ID-keyed entity caches.

`DatabaseCache::loadData()` loads base entity tables before level and join tables, then calls `db_container::finish()` to compute metrics. Relationship tables depend on these earlier entities existing:

- `unit_to_nation`
- `building_to_nation`
- `unit_to_building_level`

When adding or changing data, preserve referential integrity across the base rows, levels, nation membership, and unit-producing-building links. SQLite does not enforce every relationship that runtime loading dereferences, so a successful database open is not sufficient validation.

Level rows have additional indexing requirements:

- Global `unit_level.id` and `building_level.id` values must be dense enough for direct vector lookup.
- Every level must name an existing parent unit or building.
- Rows must be ordered into each parent as contiguous logical levels beginning at zero; runtime code indexes the resulting per-parent level vector by logical level.

## Unit And Building Metrics

Metrics are derived when the cache finishes loading, not stored as independent runtime data. Definitions and normalization weights are in:

- `game/src/player/ai/MetricDefinitions.h`
- `game/src/database/db_struct_metric.cpp`

Changing a unit or building stat can change AI matching and aggregate possession metrics. Verify that the stat remains within the expected normalized range. If it does not, intentionally adjust the metric normalization and add a focused test rather than accepting an assertion or silent distortion.

## Save Schema

Save files use SQLite tables defined in `game/src/scene/save/SQLConsts.h`. `SaveTable.h` maps those definitions to the column enums in `game/src/database/db_columns.h`. `SceneSaver` writes named columns, and `SceneLoader` validates required columns before reading explicit enum-generated column lists. The loader checks whether optional runtime tables exist before restoring them.

Age continuation is the `world_age` runtime table. It has one row with `current_age`, `age_started_tick`, and `history`; `config.map` identifies the map whose catalog must validate those IDs. Static age rows are never copied into a save. `world_age` is written by the active controller, while empty variable-length runtime tables are normally omitted. `player_levels` is mandatory and is filled with one level-0-or-higher row for every unit and building definition supported by each saved player's nation.

Saves are scene snapshots. The current saver persists mandatory unit continuation fields in `units` (including each unit's formation ID and layout slot, but not the transient acceleration result), mutable player resource statistics in `players`, player and building queues, player-wide upgrade levels, unit paths/orders, building combat state, pending commands, formations and formation orders, logical projectile state, simulation tick timing, RNG seed and named stream indexes, AI history, AI scheduler wants and lacking feedback, and consolidated MasterBrain/economy/military cached state in `ai_state`. Aim paths use one `aim_paths` row per unit with comma-separated `path` and `pending_path` cell lists; the path progress indexes remain in `units`. Player display name/color, camera position/mode, and last-period resource report values are intentionally not saved; they are GUI state and are recreated with defaults. Building deployment cells and queue capacity/nominal duration are derived from the loaded map/building definitions; queue amount and elapsed progress remain persisted. Cumulative resource totals remain because headless benchmark output uses them. Projectile rows store only target UID, remaining travel, speed, attack value, and player attribution; graphical nodes and trajectories are recreated during load. Global singleton state is stored in the one-row `config` table. Storage and refinement capacities are derived from loaded buildings and rebuilt after entity loading. Runtime tables are omitted when they have no rows. Simulation frame and seconds are derived from the persisted total tick count; wall time and the render accumulator are transient runtime state and are reset when loading. Cross-entity runtime links are stored by UID and resolved only after all entities and static grids have been rebuilt. Each unit belongs to at most one formation, so formation membership is reconstructed from `units.formation`; the runtime `Formation::units` vector is not separately persisted.

See [Save System Development](save-development.html) for the complete table reference, representative data, relationship diagram, and restoration sequence.

The current runtime databases and checked-in quicksaves already use the current age schema. The intermediate copies can be upgraded with `docs/quicksave-migration.txt`, which renames the RNG columns to `rdn_*`, normalizes the numeric schema, removes ignored columns, and creates complete `player_levels` rows. Migrate a genuinely older five-table save with `docs/old-save-migration.txt`, or regenerate the save. Saves from the immediately previous pre-consolidation format, with a four-column `config` table plus `random` and `camera` singleton tables, need `docs/config-table-migration.txt`. The loader accepts both the current flat `aim_paths` format and the previous per-cell format. The save format is currently work in progress and has no active schema-version field; extra columns in older saves, including removed GUI/reconstructable fields and legacy per-entity `level` columns, are ignored by named-column reads. A save without mandatory `player_levels` coverage is rejected.

For a save-format change:

1. Update the SQL table definition.
2. Update the matching column enum.
3. Update the save bindings and loader DTOs under `game/src/scene/save/` and `game/src/scene/load/`, preserving the same enum and binding order.
4. Decide explicitly whether existing saves must remain readable; saves that do not contain the required current tables or columns need an explicit migration or regeneration. Legacy `REAL` values in fields converted to precision-scaled integers remain readable, but newly saved databases use integer storage. There is no automatic schema-version migration.
5. Validate the required column names and generated SQL, then exercise save and load with a representative scene that includes queues if queue preservation is required.

## Save/Load Diagnostics

Save and load failures include the file path and the failing operation in the game log and standard error. A load is rejected before map or simulation construction when a required table or column is missing, `config` has zero or multiple rows, `config.precision` is invalid, `config.map` or `config.size` is invalid, or a required query cannot be prepared.

For an old save, look for messages such as:

```text
[Load Error] load 'saves/old-save.db' failed: table 'units' schema mismatch: missing columns: next_state, ...; actual columns: ...
```

For a required-table mismatch, compare the reported columns with `PRAGMA table_info(units);` and the other table named in the error. Optional-table query failures also include the generated SQL text. For the documented old five-table format, make a backup and run `docs/old-save-migration.txt` with `sqlite3.exe`, then retry the load. For the pre-consolidation format, make a backup and run `docs/config-table-migration.txt` instead. A missing required table means the file is incomplete or is not an Art of War scene save and cannot be repaired by either documented migration.

Save failures identify the target path and table or transaction operation. The save API returns `false`; a committed save with only a post-commit `VACUUM` warning remains usable. The saver does not replace an existing database: because it creates tables without `IF NOT EXISTS`, saving to an existing save file fails when the first table already exists.

## Data Change Checklist

1. Identify the authoritative database and affected table relationships.
2. Preserve IDs referenced by maps, saves, nation links, and code unless the change includes a migration. Verify level IDs and per-parent level continuity as well.
3. Update source structs, column enums, named save bindings, explicit load bindings, metrics, and runtime assets together when a schema or gameplay field changes.
4. Do not edit `*.db-wal` or `*.db-shm` files.
5. Run focused tests and a runtime smoke test from the game working directory.

Use a SQLite-aware editor or migration script that preserves the repository's database files. Do not treat the copied runtime databases or build output as the source of truth.
