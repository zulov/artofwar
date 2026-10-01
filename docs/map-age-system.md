# Map-Owned Age System

The active world-age model is intentionally small:

- A map is the mission identity for now.
- `maps.db.map.age_ids` is an ordered comma-separated list of allowed age IDs.
- `maps.db.age` defines age IDs and their logical stage.
- `maps.db.condition` defines reusable conditions.
- `maps.db.age_condition` joins one or more conditions to each age. A condition can be shared by multiple ages.
- All conditions for one age must be true on the same simulation tick.
- The age changes immediately when a candidate qualifies; there is no hold counter.
- Every age has a maximum duration of `18,000` simulation ticks. If no candidate qualifies by then, the next-stage candidate with the greatest condition progress is selected; ties use `map.age_ids` order.
- Candidate order in `map.age_ids` is the deterministic tie-break when multiple ages qualify.

## Schema

```sql
map(id, xmlName TEXT, name TEXT, age_ids TEXT)
age(id INT PRIMARY KEY, stage INT, name TEXT)
condition(id INT PRIMARY KEY, metric INT, target REAL)
age_condition(age_id INT, condition_id INT, PRIMARY KEY(age_id, condition_id))
```

`stage` groups alternative age IDs. For example, IDs `1` and `2` can both be
stage 1, while IDs `5` and `6` can both be stage 3. The numeric ID remains a
stable identifier used by level gates and save state.

Metric values are:

- `0`: average worker count
- `1`: average army count

The current catalog rows are:

| Age ID | Stage | Name | Condition |
| ---: | ---: | --- | --- |
| 0 | 0 | Age of Settlement | none |
| 1 | 1 | Age of Growth | workers >= 10 |
| 2 | 1 | Age of Mobilization | army >= 20 |
| 3 | 2 | Age of Consolidation | workers >= 20 |
| 4 | 2 | Age of Fortification | army >= 30 |
| 5 | 3 | Age of Industry | workers >= 40 |
| 6 | 3 | Age of Conquest | army >= 70 |

The starting age is ID `0`, stage `0`, and has no conditions. A map must list
age `0` first. The runtime only considers ages in the map list whose stage is
exactly one greater than the current age's stage.

## Current Data

The checked-in maps are migrated as follows:

```text
map 0: 0,1
map 1: 0,1,2,3,4,5,6
```

The map database does not store explicit transition edges. The stage difference
and the ordered `age_ids` list define the candidate transitions. A qualifying
candidate is selected immediately in list order. If the 18,000-tick timeout is
reached, the next-stage candidate with the greatest condition progress wins;
the list order resolves equal progress.

The checked-in databases are already migrated to this schema. Unit and building
level rows store their minimum logical stage in the `age_stage` column.

## Save State

Scene saves store one row in:

```sql
world_age(
    current_age INT NOT NULL,
    age_started_tick INT NOT NULL,
    history TEXT NOT NULL
)
```

`current_age` is the selected age ID, `age_started_tick` is the simulation tick
when that age was entered, and `history` stores the reached age IDs as a
comma-separated route, for example `0,2,3,4`. The route must start at `0`, use
IDs listed by the selected map, advance one logical stage at a time, and end at
`current_age`. The age start tick is restored so the timeout continues across
saves. Separate transition, hold, and history tables are no longer part of the
save format.

For example, the checked-in `saves/quicksave.db` currently contains:

```text
config.map = 1
config.total_ticks = 0
world_age.current_age = 0
world_age.age_started_tick = 0
world_age.history = '0'
```
