# Preserve Database IDs As Runtime Keys

## Status

Accepted existing constraint.

## Context

`DatabaseCache` stores ID-keyed entities in vectors indexed by database ID. Runtime code directly looks up units, buildings, nations, and resources by those IDs. Data relationships, maps, and saves may retain these references. HUD-size and HUD-variable lists are not part of this invariant; they are appended in query order.

Global level IDs are also direct vector keys. Level rows must reference an existing parent, and the per-parent logical level sequence must remain contiguous from zero. Several join-table references are dereferenced directly by the loader rather than validated by SQLite constraints.

## Decision

Treat database IDs as stable runtime keys. Renumbering or deleting referenced IDs requires an explicit migration of every affected data source and compatibility decision for existing saves. Database and save schemas are positional runtime interfaces because loaders decode `SELECT *` results by ordinal enum; column reordering needs the same coordinated change.

## Consequences

- New data must use valid IDs and relationship references.
- Schema changes must update cache loading, ordinal enums, data structs, and save/load contracts together.
- Do not infer entity identity from SQLite row order.
