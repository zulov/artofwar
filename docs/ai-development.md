# AI Development

## Scope And Starting Points

The computer-player decision flow starts in `game/src/player/ai/AiOrchestrator.cpp`:

1. `action()` runs Master, Economy, Military, and Unit brains.
2. It converts outputs into `WantList` requests for workers, units, resource buildings, and active upgrades.
3. `WantList` applies affordability and soft-reserve checks; `WantExecutor` resolves costs, queues game commands, and uses `BuildSpatialBrain` to select visible valid building positions.
4. `order()` assigns worker collection and military movement/combat orders.

The simulation schedules the action and order phases independently through `PerFrameAction::AI_ACTION` and `PerFrameAction::AI_ORDER` in `game/src/simulation/Simulation.cpp`. Each phase operates on the latest available brain outputs and state; action requests are submitted during the first action phase.

Brains receive the selected enemy player's current possession and score data. There is no scouting subsystem, but this is not fog-of-war-limited strategic information. Visibility currently restricts building-placement candidates rather than the enemy inputs supplied to brains.

## Brain Files And Dimensions

Default weights are in `game/Data/ai/`. A nation's `brain_prefix` database field provides prefixes for files in this order:

1. `master.csv`
2. `economy.csv`
3. `build_spatial.csv`
4. `unit.csv`
5. `military.csv`

Runtime consumes slots 0 through 4 only and does not validate the prefix-token count. The checked-in nation data currently has an unused sixth token; do not treat it as an active brain slot.

The constructors in `MasterBrain.cpp`, `EconomyBrain.cpp`, `BuildSpatialBrain.cpp`, `UnitBrain.cpp`, and `MilitaryBrain.cpp` assert that CSV input and output widths match their enums. Enum order is therefore part of the model-data interface. Do not reorder existing enum members.

The CSV loader is `game/src/utils/FileUtils.h`. Its first row is an input holder: its number of weights declares the input width, and its values are not evaluated. Each later row is one dense layer, with weights and biases separated by `;;`. Runtime evaluation is `tanh(W * input + bias)` in `game/src/player/ai/nn/Layer.cpp`.

Constructor assertions validate exposed input and output widths, but the loader does not fully validate malformed CSV content, intermediate layer connectivity, or zero/mismatched bias shapes. Treat CSV generation and review as part of correctness; a successful endpoint-width assertion is not a complete topology validation.

Use `tools/gen_brain_csv.ps1` to generate structurally valid random weights rather than hand-writing CSV rows. It creates untrained brains only. `BrainProvider` logs and asserts for missing or empty files; brain constructors assert declared input and output dimensions.

Example topology check:

```powershell
powershell -ExecutionPolicy Bypass -File tools\gen_brain_csv.ps1 -Check -Brains 'master.csv|34|22|9'
```

The dimensions in this example must be derived from the current enums, not copied blindly. Confirm them in source before generating or replacing a CSV.

## Changing A Brain Interface

For an input change:

1. Add the member at the intended location in the appropriate `*InputIdx` enum.
2. Populate and normalize it in that brain's `decide()` implementation.
3. Regenerate or retrain every affected CSV with the new input width.
4. Update dimension-focused tests and run the relevant test executable.

For an output change:

1. Add the output enum member and consume it in the same change.
2. Update derived constants and `static_assert`s when the output is composite.
3. Regenerate or retrain every affected CSV with the new output width.
4. Add focused tests for the new mapping or consumer behavior.

Do not update only the enum or only the CSV. Constructor assertions catch basic width mismatches, but they cannot detect an accidental semantic reorder.

## Special Invariants

- `UnitBrain` emits a 23-value unit profile plus two upgrade urgencies. `UNIT_PROFILE_SIZE` must equal `UnitMetricIdx` count; `MetricDefinitions.h` enforces this with a `static_assert`.
- Unit selection compares the profile with normalized unit metrics in `AiOrchestrator::resolveUnit()`. Metric order and normalization live in `MetricDefinitions.h` and `db_struct_metric.cpp`.
- `MilitaryBrain` emits one signed pressure for each unordered pair of `MilitaryCenterIdx` values, followed by infantry/range/cavalry preferences. Preserve this layout and `MILITARY_OUTPUT_COUNT`.
- `BuildSpatialBrain` output width must equal `AI_MAP_COUNT`. Its one-hot building placement class must stay in `BuildPlacementClass` enum order.
- `WantList` persists requests across AI actions. It boosts active requests, decays inactive requests, caps the sorted list, applies affordability/soft-reserve logic, and reports unaffordable costs back to the brains. See `WantList.cpp`, `WantListExecute.cpp`, and `WantExecutor.cpp`.

`AiOrchestrator` applies a one-hop missing-producer fallback before adding worker, unit, or unit-upgrade wants. It checks whether the player owns any building type that can ever produce the unit, not whether that building is ready. If no such instance exists, it emits the producer-building want at the same priority and does not persist the original want; a later AI action must emit it again. The executor later filters producer buildings by readiness, so a queued unit-like want can still fail for that action pass. Building upgrades become a base-building want only when no owned instance exists; the upgrade command itself has no readiness predicate.

Master `BUILDING_URGENCY`, `DEFENCE_BUILDING_URGENCY`, and direct tech-building requests are currently disabled in `AiOrchestrator.cpp`. Resource-building needs and missing-producer fallback are the live building-creation paths. `EXPAND_URGENCY` reaches `BuildSpatialBrain` only when a building is already being placed; it does not initiate an expansion request. `TECH_URGENCY` remains an input to EconomyBrain and UnitBrain.

## Focused Tests

- `tests/BrainValidationTest.cpp`: valid brain holder and endpoint-dimension behavior.
- `tests/FileUtilsTest.cpp`: CSV parsing behavior.
- `tests/LayerTest.cpp`: feed-forward layer behavior.
- `tests/WantListTest.cpp`: request merging, priority aging, affordability, and reserve behavior.
- `tests/MilitaryCommandCalculatorTest.cpp`: per-unit military target selection.
- `tests/AiHistoryTest.cpp` and `tests/AiUtilsTest.cpp`: feedback and AI utility logic.

When changing AI behavior, test the smallest affected component first. A headless game run is useful for integration behavior, but does not replace a deterministic unit test for queueing, scoring, dimensions, or target selection.

## External Genetic-Algorithm Trainer

The sibling repository `../AiTrainer` evolves CSV weights by repeatedly running Art of War matches. It is a separate Java/Maven application, not a component of this solution.

Its normal workflow is:

1. Load baseline weights from `AiTrainer/build/Data/ai/`.
2. Create candidate datasets by perturbing baseline weights for generation zero, then retaining selected parents and producing crossover/mutated children in later generations.
3. Write candidate CSVs to `AiTrainer/build/fake/<simulation>_<player>/`.
4. Start `AiTrainer/build/art_of_war.exe` headlessly with a configured save, time limit, and `-brainpath1`/`-brainpath2` overrides. Brain paths select a candidate directory only for the brain types in the selected training set; all others use `Data/ai/`.
5. Read Art of War's `result/res_<simulation>.txt`, calculate the configured fitness, and delete successful candidate directories and result files.
6. Save checkpoints and final candidates below `AiTrainer/build/result/`.

The training configuration is `AiTrainer/src/main/resources/properties.yaml`. It controls the fitness-oriented `TRAIN_SET`, match duration, population size, generations, worker processes, crossover/mutation parameters, and game flags. `TrainSet.java` maps each training set to a fitness function and the brain types it evolves. Art of War writes the score, unit/building counts, resources, and aggregate metrics used by those fitness functions in `Main::writeOutput()`.

Run the trainer from `AiTrainer`, after verifying that its `build/` directory is a complete game runtime containing the intended executable, `Data/`, `CoreData/`, `CoreDataMy/`, and the configured save. Its current Maven configuration targets Java 25, so use a compatible JDK and Maven.

### Compatibility Gate

`AiTrainer` currently matches the five Art of War brain slots: Master, Economy, Build Spatial, Unit, and Military. The trainer topology tests lock their current input/output widths and hidden widths. Future brain-interface changes must update the C++ enums, `AiTrainer/src/main/java/pl/zulov/data/BrainType.java`, and their tests in one change.

There is currently no checked-in runtime-sync script in this repository. Before training, manually or through maintained trainer-local tooling refresh `AiTrainer/build/` with the intended release executable, DLLs, assets, saves, and five active brain CSVs. Training against a stale copied executable or stale baseline weights can produce incompatible candidates.

At minimum, verify for every active brain:

1. The trainer's file name and slot order match the five prefixes consumed by the game: Master, Economy, Build Spatial, Unit, Military.
2. Trainer input/output dimensions match the corresponding C++ enums and constructor assertions.
3. Trainer topology assumptions match the actual checked-in CSV topology.
4. `BrainTypeTest` and `RepositoryTest` pass after updating the trainer, then run an Art of War smoke match using the generated candidate files.

Do not copy final candidates directly into `game/Data/ai/` until they pass this compatibility gate and an integration match. Preserve the trainer output directory with its logs/configuration so results remain reproducible.
