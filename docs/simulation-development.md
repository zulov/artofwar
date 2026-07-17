# Simulation Development

## Fixed-Step Contract

`Simulation::update()` accumulates real time and executes fixed simulation ticks while the accumulator is at least `TIME_PER_UPDATE`. The value is `0.033333333f`, approximately 30 updates per second, in `game/src/simulation/SimGlobals.h`.

Gameplay changes must be reasoned about in fixed ticks, not rendering frames. The loop can run more than one simulation tick for a single render update.

## Tick Order

The current order in `game/src/simulation/Simulation.cpp` is:

1. Apply pending creation and upgrade commands.
2. Run object AI and scheduled player AI action/order phases.
3. Execute queued actions.
4. Calculate forces and apply acceleration.
5. Move units, update projectiles, execute unit states, and apply state transitions.
6. Advance production queues and update influence/visibility maps.
7. Remove entities from grids, dispose deleted entities, update players, and update formations.

Do not move a step without checking the data it reads and invalidates. For example, AI queues commands before action execution; queues can add upgrade commands for a later creation/upgrade pass; movement invalidates environment caches before later users consume them.

## Commands, State, And Queues

- `ActionCenter` owns deferred creation, upgrade, and unit-action processing for the simulation.
- Building and player queues advance in `updateQueues()`. Completion can enqueue unit creation, upgrade, building completion, or resource creation.
- Unit behavior is state-driven. State actions run before `StateManager::executeChange()` applies transitions.
- Environment grids, influence maps, and visibility are shared simulation state. Keep their refresh and invalidation order valid when adding movement, spawning, disposal, or new spatial queries.

## Player AI Scheduling

`Simulation::aiPlayers()` skips player AI when `SIM_GLOBALS.NO_PLAYER_AI` is set. Otherwise it runs AI for non-active players, or all players when `SIM_GLOBALS.ALL_PLAYER_AI` is set. `FrameInfo` controls whether the action and order phases run on a given fixed tick.

Relevant launch flags are parsed in `game/src/Main.cpp`: `-headless`, `-benchmark`, `-allplayerai`, `-noplayerai`, `-faketerrain`, `-savename`, `-timelimit`, `-brainpath1`, `-brainpath2`, and `-random`.

## Change Checklist

1. Identify the earliest tick phase where inputs are valid and the latest phase where outputs are still safe to consume.
2. Use `TIME_PER_UPDATE` for simulation-time movement, cooldowns, projectiles, and state work. Do not use renderer frame time for gameplay progression.
3. Queue creation, upgrade, or player commands through the established command path instead of directly mutating collections during iteration.
4. Update grids, caches, influence, visibility, queues, and disposal behavior for any new object lifecycle or spatial mutation.
5. Add a focused unit test when the behavior is deterministic. Exercise a game or headless smoke test for cross-system ordering.
