# Preserve Database IDs As Runtime Keys

## Status

Accepted existing constraint.

## Context

`DatabaseCache` stores loaded entities in vectors indexed by database ID. Runtime code directly looks up units, buildings, nations, and resources by those IDs. Data relationships, maps, and saves may retain these references.

## Decision

Treat database IDs as stable runtime keys. Renumbering or deleting referenced IDs requires an explicit migration of every affected data source and compatibility decision for existing saves.

## Consequences

- New data must use valid IDs and relationship references.
- Schema changes must update cache loading, data structs, and save/load contracts together.
- Do not infer entity identity from SQLite row order.
