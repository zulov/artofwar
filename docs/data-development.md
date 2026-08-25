# Data Development

## Runtime Databases

All paths are relative to the game runtime directory, not the repository root. `DatabaseCache` loads:

- `Data/Database/base.db`: UI, graphics, and settings data. It is skipped in headless mode.
- `Data/Database/data.db`: nations, units, buildings, resources, levels, and their relationships.
- `Data/map/maps.db`: map metadata.
- `saves/<name>.db`: a scene save created by `SceneSaver`; `SceneLoader` receives the filename, including its `.db` suffix.

The loading implementation is `game/src/database/DatabaseCache.cpp`; save loading is `game/src/scene/load/SceneLoader.cpp`.

All three runtime databases and save tables are positional interfaces. The cache and save loader use `SELECT *` and decode result columns through ordinal enums. Changing a column's name is harmless to this code, but inserting, removing, or reordering a column without updating every matching enum and binding silently decodes the wrong fields.

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

Save files use SQLite tables defined in `game/src/scene/save/SQLConsts.h`. Column-order enums in `game/src/database/db_columns.h` are consumed by positional save/load bindings. The existing `static_assert`s check enum counts only; they do not inspect SQLite schemas or prove column order compatibility.

Saves are partial scene snapshots. They persist units, buildings, resources, players, and basic config, but not active building/player queues, simulation time, AI history, or MasterBrain delta state. A load can therefore resume entities while discarding in-progress work and producing a first-AI-cycle delta discontinuity.

Current limitation: `Main::save()` passes map ID `1` to every new save. A save made from any other map does not preserve its selected map. Also, `quicksave512.db` uses a legacy column order that the current positional loader cannot read correctly; do not use it as a compatibility fixture until it is migrated or the loader gains schema-version handling.

For a save-format change:

1. Update the SQL table definition.
2. Update the matching column enum.
3. Update the save bindings and loader DTOs under `game/src/scene/save/` and `game/src/scene/load/`, preserving the same ordinal order.
4. Decide explicitly whether existing saves must remain readable; there is no automatic migration or schema-version layer.
5. Validate the schema order, then exercise save and load with a representative scene that includes queues if queue preservation is required.

## Data Change Checklist

1. Identify the authoritative database and affected table relationships.
2. Preserve IDs referenced by maps, saves, nation links, and code unless the change includes a migration. Verify level IDs and per-parent level continuity as well.
3. Update source structs, column enums, positional bindings, metrics, and runtime assets together when a schema or gameplay field changes.
4. Do not edit `*.db-wal` or `*.db-shm` files.
5. Run focused tests and a runtime smoke test from the game working directory.

Use a SQLite-aware editor or migration script that preserves the repository's database files. Do not treat the copied runtime databases or build output as the source of truth.
