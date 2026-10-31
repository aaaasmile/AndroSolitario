# Invido Project Instructions

## Project
- This is the Invido card game, written in C++ and built on SDL3. The existing app shares common infrastructure with Solitario, but the Invido and Solitario game-specific sources are selected at build time; do not combine them into one executable unless explicitly requested.
- Read [`passaggi_invido.md`](../passaggi_invido.md) for project-specific architecture, build, and debugging notes before changing game-specific code.
- Keep changes focused and preserve existing interfaces and conventions. Do not modify vendored SDL sources unless the task explicitly requires it.

## C++ and SDL
- Follow the repository's `.clang-format` and the surrounding C++ style. Prefer clear ownership, safe lifetimes, and existing project abstractions over new dependencies or broad refactors.
- Respect the existing callback and game-flow contracts between `InvidoCore`, `Mano`, `Giocata`, and `Partita`; trace callers before changing these interfaces.
- Keep Invido-only code under `app/jni/src/GameGfx/Invido` and use the existing CMake build selection rather than adding game-specific behavior to shared code unnecessarily.

## Build and Test
- Windows/MySys2 Invido build (from the repository root; adjust `cmake`/Ninja paths to the configured environment):
  ```powershell
  cmake -S app/jni -B buildinvido -DSDL_WAYLAND=OFF -DINVIDOPRJ=ON -DBUILD_INVIDO_TESTS=ON
  cmake --build buildinvido
  .\buildinvido\InvidoTests.exe
  ```
- The Invido test target is controlled by `BUILD_INVIDO_TESTS`; the CMake target is `InvidoTests` (`InvidoTests.exe` on Windows). Run the focused tests after game-core changes, then build the Invido app when practical.
- For a test failure, report the exact command and relevant output. Do not claim tests passed unless they were run.

## Agent Workflow
- Inspect the relevant code and project notes before editing; make the smallest complete change that solves the task.
- Avoid deleting or recreating build directories unless necessary. Never remove user changes or generated data without explicit need.
- Summarize changed files and verification performed, including any build/test limitations.
