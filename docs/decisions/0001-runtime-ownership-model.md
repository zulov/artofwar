# Keep The Existing Runtime Ownership Model

## Status

Accepted existing constraint.

## Context

`Game` is a service locator for major runtime systems. It stores raw pointers but does not own or delete the registered services when `Game::dispose()` deletes the locator itself. Some simulation state is global through `SIM_GLOBALS`.

Lifecycle is deliberately distributed: `Main` creates and tears down registered managers such as environment, players, camera, formations, and palettes; `Simulation` owns its `ActionCenter` and object manager; `UnitFactory` owns the global state manager. Initialization and teardown changes must follow the actual creator/destructor path rather than assuming `Game` owns a registered pointer.

## Decision

Feature work preserves this ownership and lifecycle model. New code should use the established service access, creator, and teardown paths when it belongs to an existing manager.

## Consequences

- Do not introduce isolated smart-pointer ownership into an existing raw-pointer service graph without defining the complete lifecycle boundary.
- Do not replace the service locator or global simulation state as incidental cleanup in a gameplay change.
- A migration is possible only as an explicitly scoped refactor with initialization, destruction, and test coverage planned together.
- Do not infer ownership from a `Game::set*` call; it registers access, not lifetime management.
