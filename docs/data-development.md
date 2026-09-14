# Data Development

## Runtime Databases

All paths are relative to the game runtime directory, not the repository root. `DatabaseCache` loads:

- `Data/Database/base.db`: UI, graphics, and settings data. It is skipped in headless mode.
- `Data/Database/data.db`: nations, units, buildings, resources, levels, and their relationships.
- `Data/map/maps.db`: map metadata.
- `saves/<name>.db`: a scene save created by `SceneSaver`; `SceneLoader` receives the filename, including its `.db` suffix.

The loading implementation is `game/src/database/DatabaseCache.cpp`; save loading is `game/src/scene/load/SceneLoader.cpp`.

The runtime data databases and save tables use different query contracts. `DatabaseCache` still reads data tables with `SELECT *` and decodes result columns through ordinal enums. Save writes use named-column `INSERT`s generated from the save enums, while save loads validate required column names and generate explicit `SELECT` lists from those enums. For saves, SQLite physical column order and extra columns do not matter, but expected column names, enum order, and bindings must stay aligned. For `DatabaseCache` tables, inserting, removing, or reordering a column still requires updating the matching enum and binding.

`DatabaseCache` reads `Data/Database/base.db`, but its current settings-write paths open `Data/base.db` and target `graphics_settings` while the loaded table is `graph_settings`. Treat graphics/settings persistence as a known limitation until those paths are corrected; do not rely on UI changes being written back to the loaded database.

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

Saves are scene snapshots. The current saver persists mandatory unit continuation fields in `units` (including each unit's formation ID and layout slot, but not the transient acceleration result), mutable player resource statistics in `players`, player and building queues, player-wide upgrade levels, unit paths/orders, building combat state, pending commands, formations and formation orders, logical projectile state, simulation tick timing, RNG seed and named stream indexes, AI history, AI scheduler wants and lacking feedback, and consolidated MasterBrain/economy/military cached state in `ai_state`. Player display name/color, camera position/mode, and last-period resource report values are intentionally not saved; they are GUI state and are recreated with defaults. Building deployment cells and queue capacity/nominal duration are derived from the loaded map/building definitions; queue amount and elapsed progress remain persisted. Cumulative resource totals remain because headless benchmark output uses them. Projectile rows store only target UID, remaining travel, speed, attack value, and player attribution; graphical nodes and trajectories are recreated during load. Global singleton state is stored in the one-row `config` table. Storage and refinement capacities are derived from loaded buildings and rebuilt after entity loading. Runtime tables are omitted when they have no rows. Simulation frame and seconds are derived from the persisted total tick count; wall time and the render accumulator are transient runtime state and are reset when loading. Cross-entity runtime links are stored by UID and resolved only after all entities and static grids have been rebuilt. Each unit belongs to at most one formation, so formation membership is reconstructed from `units.formation`; the runtime `Formation::units` vector is not separately persisted.

See [Save System Development](save-development.html) for the complete table reference, representative data, relationship diagram, and restoration sequence.

Current limitation: the checked-in `quicksave.db`, `quicksave256.db`, and `quicksave512.db` files use the legacy five-table schema and are missing current required columns in `units`, `buildings`, `players`, and `config`. The loader rejects them before map or simulation construction; their physical column order is not the problem. Migrate a copy with `docs/old-save-migration.txt` or regenerate the saves. Saves from the immediately previous pre-consolidation format, with a four-column `config` table plus `random` and `camera` singleton tables, need `docs/config-table-migration.txt`. The save format is currently work in progress and has no active schema-version field; extra columns in older saves, including removed GUI/reconstructable fields, are ignored by named-column reads.

For a save-format change:

1. Update the SQL table definition.
2. Update the matching column enum.
3. Update the save bindings and loader DTOs under `game/src/scene/save/` and `game/src/scene/load/`, preserving the same enum and binding order.
4. Decide explicitly whether existing saves must remain readable; saves that do not contain the required current tables or columns need an explicit migration or regeneration. There is no automatic schema-version migration.
5. Validate the required column names and generated SQL, then exercise save and load with a representative scene that includes queues if queue preservation is required.

## Save/Load Diagnostics

Save and load failures include the file path and the failing operation in the game log and standard error. A load is rejected before map or simulation construction when a required table or column is missing, `config` has zero or multiple rows, `config.precision` is invalid, `config.map` or `config.size` is invalid, or a required query cannot be prepared.

For an old save, look for messages such as:

```text
[Load Error] load 'saves/quicksave512.db' failed: table 'units' schema mismatch: missing columns: next_state, ...; actual columns: ...
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
