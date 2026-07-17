# Data Development

## Runtime Databases

All paths are relative to the game runtime directory, not the repository root. `DatabaseCache` loads:

- `Data/Database/base.db`: UI, graphics, and settings data. It is skipped in headless mode.
- `Data/Database/data.db`: nations, units, buildings, resources, levels, and their relationships.
- `Data/map/maps.db`: map metadata.
- `saves/<name>`: a saved scene, loaded separately by `SceneLoader`.

The loading implementation is `game/src/database/DatabaseCache.cpp`; save loading is `game/src/scene/load/SceneLoader.cpp`.

## ID And Relationship Invariants

Runtime cache vectors are indexed by database ID. `setEntity()` resizes a vector and stores each entity at `array[id]`; accessors such as `getUnit(id)` directly index that vector. A reference must therefore name an existing row with the intended ID. Never rely on SQL result position as identity.

`DatabaseCache::loadData()` loads base entity tables before level and join tables, then calls `db_container::finish()` to compute metrics. Relationship tables depend on these earlier entities existing:

- `unit_to_nation`
- `building_to_nation`
- `unit_to_building_level`

When adding or changing data, preserve referential integrity across the base rows, levels, nation membership, and unit-producing-building links.

## Unit And Building Metrics

Metrics are derived when the cache finishes loading, not stored as independent runtime data. Definitions and normalization weights are in:

- `game/src/player/ai/MetricDefinitions.h`
- `game/src/database/db_struct_metric.cpp`

Changing a unit or building stat can change AI matching and aggregate possession metrics. Verify that the stat remains within the expected normalized range. If it does not, intentionally adjust the metric normalization and add a focused test rather than accepting an assertion or silent distortion.

## Save Schema

Save files use SQLite tables defined in `game/src/scene/save/SQLConsts.h`. Column-order enums in `game/src/database/db_columns.h` are checked against the save schema with `static_assert`s.

For a save-format change:

1. Update the SQL table definition.
2. Update the matching column enum.
3. Update the save bindings and loader DTOs under `game/src/scene/save/` and `game/src/scene/load/`.
4. Decide explicitly whether existing saves must remain readable; there is no automatic migration layer.
5. Exercise save and load with a representative scene.

## Data Change Checklist

1. Identify the authoritative database and affected table relationships.
2. Preserve IDs referenced by maps, saves, nation links, and code unless the change includes a migration.
3. Update source structs, column enums, metrics, and runtime assets together when a schema or gameplay field changes.
4. Do not edit `*.db-wal` or `*.db-shm` files.
5. Run focused tests and a runtime smoke test from the game working directory.

Use a SQLite-aware editor or migration script that preserves the repository's database files. Do not treat the copied runtime databases or build output as the source of truth.
