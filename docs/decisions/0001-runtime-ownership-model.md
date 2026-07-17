# Keep The Existing Runtime Ownership Model

## Status

Accepted existing constraint.

## Context

`Game` is a service locator for major runtime systems. Initialization and teardown are staged in `Main.cpp`; many services are held as raw pointers and some simulation state is global through `SIM_GLOBALS`.

## Decision

Feature work preserves this ownership and lifecycle model. New code should use the established service access and teardown paths when it belongs to an existing manager.

## Consequences

- Do not introduce isolated smart-pointer ownership into an existing raw-pointer service graph without defining the complete lifecycle boundary.
- Do not replace the service locator or global simulation state as incidental cleanup in a gameplay change.
- A migration is possible only as an explicitly scoped refactor with initialization, destruction, and test coverage planned together.
