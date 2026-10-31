---
name: invido-cpp-sdl3
description: 'Use when implementing, debugging, or testing the Invido game in this C++/SDL3 workspace, especially game flow, callbacks, CMake builds, and Invido unit tests.'
---

# Invido C++/SDL3 Development

Use this workflow for Invido-specific implementation and debugging. For general workspace guidance, follow [the project instructions](../../copilot-instructions.md). Consult [`passaggi_invido.md`](../../../passaggi_invido.md) for the maintainer's detailed build and architecture notes.

## Workflow

1. Identify whether the change belongs to shared app code or `app/jni/src/GameGfx/Invido`; keep game-specific behavior in the Invido sources.
2. Before changing a game-flow method, callback, or interface, follow its callers and related state transitions across `InvidoCore`, `Mano`, `Giocata`, and `Partita`. Preserve callback lifetimes and existing contracts.
3. Make a small, focused change. Follow `.clang-format`, nearby conventions, and existing CMake target selection. Avoid unrelated changes to Solitario or vendored SDL code.
4. Configure and build from the repository root for Windows/MySys2:
   ```powershell
   cmake -S app/jni -B buildinvido -DSDL_WAYLAND=OFF -DINVIDOPRJ=ON -DBUILD_INVIDO_TESTS=ON
   cmake --build buildinvido
   ```
   Reconfigure only when build options or configuration need to change; do not delete an existing build directory by default.
5. Run the tests:
   ```powershell
   .\buildinvido\InvidoTests.exe
   ```
   For a focused issue, also run the relevant test or debugger configuration documented in `passaggi_invido.md`.
6. Inspect the final diff and report the behavior changed, commands run, and any unverified cases. Never report a test as passing unless it ran successfully.

## Troubleshooting

- If CMake configuration or compilation fails, retain the first actionable error and distinguish environment/toolchain problems from code failures.
- Invido tests may stop at the first assertion; report the failing test and assertion rather than treating the executable's early exit as a successful run.
- Do not change build flags, test semantics, or shared game selection as a workaround without confirming the intended behavior.
