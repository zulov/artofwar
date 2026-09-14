# Preserve Database IDs As Runtime Keys

## Status

Accepted existing constraint.

## Context

`DatabaseCache` stores ID-keyed entities in vectors indexed by database ID. Runtime code directly looks up units, buildings, nations, and resources by those IDs. Data relationships, maps, and saves may retain these references. HUD-size and HUD-variable lists are not part of this invariant; they are appended in query order.

Global level IDs are also direct vector keys. Level rows must reference an existing parent, and the per-parent logical level sequence must remain contiguous from zero. Several join-table references are dereferenced directly by the loader rather than validated by SQLite constraints.

## Decision

Treat database IDs as stable runtime keys. Renumbering or deleting referenced IDs requires an explicit migration of every affected data source and compatibility decision for existing saves. Database data tables and save tables have different schema contracts: data-table loaders decode `SELECT *` results by ordinal enum, while save writes use named columns and save loads use explicit enum-generated `SELECT` lists. Save enum names and order still form a C++ runtime interface, but SQLite physical column reordering does not by itself break the current save loader. Singleton global continuation state belongs in the one-row `config` table; variable-length state remains in dedicated runtime tables. The current save format has no active schema-version field; structural changes require an explicit migration or regeneration when old saves must remain readable.

## Consequences

- New data must use valid IDs and relationship references.
- Schema changes must update cache loading, ordinal enums, data structs, and save/load contracts together.
- Do not infer entity identity from SQLite row order.
