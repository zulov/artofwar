# Art of War Project Context

## Purpose

Art of War is a Windows real-time strategy game written in C++ and built on Urho3D. It simulates terrain, resources, buildings, units, combat, UI, save games, and computer-controlled players. AI decisions are driven by feed-forward neural-network "brains" loaded from CSV weights.

## Repository Layout

- `game/src/`: game code. Main areas are simulation, objects, commands, player/ai, environment, scene loading/saving, HUD, controls, camera, and database cache.
- `game/Data/`: runtime content: maps, UI XML, localization, SQLite databases, save games, and AI CSV weights.
- `game/CoreData/` and `game/CoreDataMy/`: Urho3D core and custom rendering assets.
- `game/include/` and `game/lib/`: bundled dependencies and libraries, including Urho3D, Eigen, SQLite, `fast_float`, `magic_enum`, and `exprtk`.
- `tests/`: GoogleTest unit tests. The test project references the game project.
- `docs/`: AI, economy, targeting, and influence-map design references. Treat implementation as authoritative when source and docs disagree.
- `tools/gen_brain_csv.ps1`: creates dimensionally valid random/untrained AI-weight CSV files.

## Architecture

- `game/src/Main.cpp` initializes Urho3D, global `Game` services, resources/UI, database cache, players, terrain/environment, simulation, and influence maps.
- `game/src/Game.h` is a service locator with raw pointers to major managers. `SIM_GLOBALS` and similar global state are established patterns in this codebase.
- `game/src/simulation/Simulation.cpp` is the main fixed-step simulation loop at 30 updates per second. It processes queued commands, unit behavior/combat, queues, visibility/influence maps, players, and formations.
- Runtime gameplay definitions come from SQLite files under `game/Data/Database/` and `game/Data/map/`; scene saves are SQLite files under `game/saves/`.
- AI is coordinated by `game/src/player/ai/AiOrchestrator.cpp`. Master, economy, unit, military, and spatial brains produce priorities and orders. `WantList` prioritizes affordable creation/upgrade requests and `WantExecutor` issues commands.
- Neural-network layers use `tanh(W * input + bias)`. Brain CSV files under `game/Data/ai/` must match the expected dimensions; missing or malformed files assert after logging.

## Build And Test

- Open `artofwar.sln` with Visual Studio 2022 or use MSBuild. Game configurations are x64 only; the expected toolset is MSVC `v145` and the language standard is C++23.
- Build the solution with: `msbuild artofwar.sln /m /p:Configuration=Debug /p:Platform=x64`
- The test executable is a GoogleTest console application. The project uses a legacy NuGet `packages.config`; restore packages if the GoogleTest targets are missing.
- Run the game from the game project/output working directory so relative paths to `Data`, `CoreData`, and `CoreDataMy` resolve. Do not assume the repository root is a valid runtime working directory.
- Prefer focused unit tests under `tests/` for logic changes. There is no CMake or CI configuration.

## Engineering Conventions

- Follow `.clang-format`: LLVM-derived style, tabs at width 4, 120-column limit, left-aligned pointers, and namespace indentation.
- Preserve existing ownership and lifecycle conventions unless a task explicitly calls for a broader refactor. Raw pointers, assertions, ID-indexed data, and global services are intentional existing patterns.
- Keep database schema/data changes, content assets, and code changes consistent. Verify ID/index assumptions and metric ranges when changing gameplay definitions.
- Do not edit generated build output, local IDE files, copied DLLs, package directories, or database WAL/SHM files.
- Check `git status` before editing. The working tree may contain unrelated user changes; never overwrite or revert them.

## Documentation And Discovery

- Documentation comes first: before exploring source, proposing an implementation, or editing files for any non-trivial task, discover and read every `docs/**/*.md` file. This includes newly added Markdown files.
- Start with the smallest relevant module rather than scanning the whole repository.
- Use `docs/ai-development.md`, `docs/data-development.md`, and `docs/simulation-development.md` for task-specific invariants and change checklists.
- `docs/decisions/` records durable constraints. Read the relevant record before proposing a broad architectural change.
- AI reference docs include `docs/ai_system_overview.html`, `docs/ai_orchestrator.html`, `docs/army_targeting_flow.html`, and `docs/influence_maps_deep_dive.html`.
- These HTML diagrams document intent but can lag code. Confirm behavior in source, tests, enums, and constructor validation before making implementation decisions.
- For a new feature or bug, trace the applicable flow through initialization, simulation/commands, runtime data, and tests before modifying it.
