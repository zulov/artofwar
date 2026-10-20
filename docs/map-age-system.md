# Map-Owned Age System

The active world-age model is intentionally small:

- A map is the mission identity for now.
- `maps.db.map.age_ids` is an ordered comma-separated list of allowed age IDs.
- `maps.db.age` defines age IDs and their logical stage.
- `maps.db.age_condition` stores each age's metric and threshold directly; multiple rows for an age are conjunctive.
- After an age is reached, every map-listed age with `stage = current stage + 1` is a candidate, regardless of which age was selected previously.
- All age conditions for one age must be true on the same simulation tick.
- The age changes immediately when a candidate qualifies; there is no hold counter.
- Every age has a maximum duration of `18,000` simulation ticks. If no candidate qualifies by then, the next-stage candidate with the greatest condition progress is selected; ties use `map.age_ids` order.
- Candidate order in `map.age_ids` is the deterministic tie-break when multiple ages qualify.

## Schema

```sql
map(id, xmlName TEXT, name TEXT, age_ids TEXT, goal_ids TEXT)
age(id INT PRIMARY KEY, stage INT, name TEXT) -- localization key, e.g. age_3
age_condition(age_id INT, metric INT, target REAL, PRIMARY KEY(age_id, metric, target))
```

`stage` groups alternative age IDs. The next candidates are all map-listed ages
whose stage is exactly one greater than the current age's stage. Earlier age
choices do not restrict later candidates: from either age `1` or `2`, both ages
`3` and `4` are available. Candidate ordering in `map.age_ids` resolves ties.
The numeric age ID remains a stable identifier used by level gates and save state.

Metric values are:

- `0`: average worker count
- `1`: average army count

The current catalog rows are:

| Age ID | Stage | Localization key | Display name (English) | Condition |
| ---: | ---: | --- | --- | --- |
| 0 | 0 | `age_0` | I - Settlement | none |
| 1 | 1 | `age_1` | II - Growth | workers >= 10 |
| 2 | 1 | `age_2` | II - Mobilization | army >= 20 |
| 3 | 2 | `age_3` | III - Consolidation | workers >= 20 |
| 4 | 2 | `age_4` | III - Fortification | army >= 30 |
| 5 | 3 | `age_5` | IV - Industry | workers >= 40 |
| 6 | 3 | `age_6` | IV - Conquest | army >= 70 |

The starting age is ID `0`, stage `0`, and has no age conditions. The `age.name` value is a localization key loaded from `Data/lang/top_language.json`; the localized text contains the display name and Roman stage number. A map must list
age `0` first. The runtime considers only map-listed ages whose stage is the
next stage after the current age.

## Current Data

The checked-in maps are migrated as follows:

```text
map 0: 0,1
map 1: 0,1,2,3,4,5,6
```

The ordered `age_ids` list supplies the deterministic tie-break when multiple
next-stage candidates qualify on the same tick. A qualifying candidate is
selected immediately in list order. If the 18,000-tick timeout is reached, the
next-stage candidate with the greatest condition progress wins; the map list
order resolves equal progress.

The checked-in databases are already migrated to this schema. Unit and building
level rows store their minimum logical stage in the `age_stage` column.

## Mission Goals

The map ID is also the mission ID. `mission_goal` rows in `maps.db` describe the
mission's objectives; they are not copied into saves. All configured objectives
must be true simultaneously for victory. Objectives are evaluated for the active
player each simulation tick, and their HUD rows reflect current truth. Defeat
occurs when that active player has no living units and no living buildings; it
takes priority over victory on the same tick. `-timelimit` remains an independent
training-run limit.

See [mission-goals-migration.sql](mission-goals-migration.sql) for the schema,
string type tokens and sample objectives. Objective definitions are reusable rows
in `mission_goal`; comma-separated `map.goal_ids` attaches them to maps. Age
requirements are configured separately through `age_condition`. Unit kills are saved in
`players.unit_kills`, cumulative resource values use the existing `players.sum_*`
columns, and a score-lead objective stores its resolved deadline result and both
scores so it remains fixed after that tick and across saves. Current stock,
score, building counts, elimination and objective time are derived from saved
world/player data and simulation ticks.

`mission_goal.type` stores enum-name strings, not ordinal numbers. Supported tokens
are `ELIMINATE_PLAYER`, `ELIMINATE_TEAM`, `ELIMINATE_ALL`, `ELIMINATE_UNITS`,
`ELIMINATE_BUILDINGS`, `SURVIVE_WITH_BUILDING_UNTIL`, `SURVIVE_UNTIL`,
`KILL_UNITS`, `SCORE_LEAD_AT_TIME`, `RESOURCE_STOCK`, `RESOURCE_GATHERED`,
`BUILDING_COUNT`, and `SCORE_REACH`. The migration keeps existing active map
objectives and adds unused catalog examples
for the other objective kinds. Kill counters are stored in `players.unit_kills`;
gathered-resource totals use existing `players.sum_*` (cumulative total received,
including passive income), so new saves do not need a separate mission-counter table.

### Goal Data Examples

Each `mission_goal` row defines an objective once; `map.goal_ids` assigns
objectives to a map, e.g. `1,2,5`. `id` is a global reusable goal ID. `target_player`
is a player ID or `-1` for the default opponent/all-opponent behavior. `target_id`
is interpreted by goal type: resource ID for resource objectives, building ID for
building objectives, or `-1` for any building. Resource IDs are `0` food, `1`
wood, `2` stone, and `3` gold. `target_value` is the required quantity/count/
score, and `time_ticks` is a simulation tick count (`18000` = ten minutes).

| Type token | Example parameters | Meaning |
| --- | --- | --- |
| `ELIMINATE_PLAYER` | `target_player=1` | Eliminate the specific player with ID 1. |
| `ELIMINATE_TEAM` | `target_player=1` | Eliminate the whole team belonging to player 1. |
| `ELIMINATE_ALL` | defaults | Eliminate every opposing team. |
| `ELIMINATE_UNITS` | defaults | Eliminate all enemy units. |
| `ELIMINATE_BUILDINGS` | defaults | Eliminate all enemy buildings. |
| `SURVIVE_WITH_BUILDING_UNTIL` | `target_id=-1`, `time_ticks=18000` | Survive ten minutes while owning any completed building. Set a building ID to require that specific building type. |
| `SURVIVE_UNTIL` | `time_ticks=18000` | Survive until ten minutes have elapsed. |
| `KILL_UNITS` | `target_value=20` | Defeat 20 enemy units cumulatively. |
| `SCORE_LEAD_AT_TIME` | `target_player=-1`, `time_ticks=18000` | Have a higher score than the default opponent at ten minutes. Set `target_player` to compare with a specific player. |
| `RESOURCE_STOCK` | `target_id=3`, `target_value=500` | Have 500 gold in current stock. |
| `RESOURCE_GATHERED` | `target_id=1`, `target_value=1000` | Receive a cumulative 1000 wood, even if later spent. Existing `sum_wood` includes passive income as well as worker gathering. |
| `BUILDING_COUNT` | `target_id=0`, `target_value=2` | Own two completed buildings of type ID 0 (houses). Use `target_id=-1` for any building types. |
| `SCORE_REACH` | `target_value=1000` | Reach a score of 1000. |

The checked-in catalog includes examples for every token. `goal_ids='0,1'` is
assigned to map 0, and `goal_ids='2,3,4'` to map 1; the remaining rows demonstrate
reusable definitions and do not change playable mission objectives until listed
in a map's `goal_ids`. For example, the active map-0 goals require both surviving
ten minutes with a completed building and gathering 500 food.

The `age_condition` table is exclusively for age progression; it does not add
mission victory requirements. For example, age 1
requires an average of 10 workers, while age 2 requires an average army of 20.
Mission victory requirements are defined separately by `mission_goal` and
`map.goal_ids`.

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
