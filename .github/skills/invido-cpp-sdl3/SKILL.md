---
name: invido-cpp-sdl3
description: 'Use when implementing, debugging, or testing the Invido game in this C++/SDL3 workspace, especially its Rust-like trait pattern, callback delegation, game-state progression and action queues, CMake builds, and unit tests.'
---

# Invido C++/SDL3 Development

Use this workflow for Invido-specific implementation and debugging. For general workspace guidance, follow [the project instructions](../../copilot-instructions.md). Consult [`passaggi_invido.md`](../../../passaggi_invido.md) for the maintainer's detailed build and architecture notes.

## Member Naming

For Invido class members, use a leading underscore and omit legacy type affixes such as `m_i` or `m_b`. Non-pointer, non-vector member names start with a lowercase letter after the underscore and use camel case; for example, `m_iNumManiWon` becomes `_numManiWon`. Pointer members use the `_p_` prefix followed by the member name; for example, `m_pCoreGame` becomes `_p_CoreGame`. For `std::vector` members, use `_vct` followed by an upper-case name; for example, `std::vector<CardSpec> _cpuCards` becomes `std::vector<CardSpec> _vctCpuCards`.

## Invido Traits and Callbacks

Invido uses a C-style callback-table pattern called “traits,” inspired by Rust traits but implemented with function pointers and a context pointer (not Rust interfaces or C++ inheritance). Read [`InvidoTraits.h`](../../../app/jni/src/GameGfx/Invido/Core/InvidoTraits.h) for the callback-table definitions.

For the `Giocata` trait, `VGiocataCb` declares `GiocataStarted`, `GiocataEndedWinner`, and `GiocataEndDueced`. `GiocataCb` pairs a pointer to that function table (`tc`) with a `self` context pointer. This lets `Giocata` invoke callbacks on its owner without depending directly on its implementation.

The wiring and call flow is:

1. `InvidoCore::prepGiocataDelegator()` creates a `static const VGiocataCb` table whose functions forward to `InvidoCore` methods. The table must outlive the function because `Giocata` stores its pointer.
2. `InvidoCore::Init()` passes the resulting `GiocataCb` to `_giocata.SetGiocataCB(...)`, connecting the `Giocata` member to the `InvidoCore` callbacks via the context pointer.
3. When `Giocata` processes queued actions, it calls the matching function through `_giocataCb.tc`, passing `_giocataCb.self` as the first argument. For example, a start action calls `GiocataStarted(self, playerIx)`.
4. The same pattern is used by `Mano`: `VManoCb` declares callbacks for ending a hand, notifying players, changing the trick score, handling `A Monte`, and other game events. `InvidoCore::prepManoDelegator()` creates the callback table, and `InvidoCore::Init()` connects it with `_mano.SetManoCB(...)`. `Mano` calls the functions through `_manoCb.tc`, passing `_manoCb.self`; it no longer depends directly on `InvidoCore`. Its `MatchStateWithScore` dependency remains separately injected with `SetScore()`.
5. Follow the table definition and each binding function when changing a callback. Keep signatures in sync, preserve the static table's lifetime, and ensure the context points to a live owner. Apply the same tracing approach to the `Partita` callback table.

## Game-State Coordination

`InvidoCore` is the coordinator of the game state. It owns the `Partita`, `Giocata`, and `Mano` state-machine objects as members and coordinates their transitions through callbacks and actions. Keep their responsibilities distinct when changing game logic:

- **`Partita`** tracks whether the overall game is ongoing or has ended. It is the lifetime/state boundary for the match-level game.
- **`Giocata`** represents a card-distribution trick: a new trick begins with cards dealt to the players (three cards each in the standard two-player game), and the trick eventually ends with a winner or without one. `A Monte` invalidates the current trick, so play can continue with another distribution/trick.
- **`Mano`** tracks the play and score-call (`chiamata`) state within a trick, including player turns, responses to calls, and score changes. A score raise requires the other player to respond: they may accept, raise to a higher score, or call `A Monte`, which invalidates the current `Giocata` and leads to another trick.

The default game has two players. Do not assume other player counts unless the relevant configuration or task explicitly enables them. When modifying transitions, verify how `Mano` notifies `InvidoCore`, how `InvidoCore` starts/ends `Giocata`, and how a trick outcome affects `Partita` and match scoring; avoid duplicating state in multiple classes.

## Match Progression and Action Queues

Match progression is advanced by repeated calls to `InvidoCore::NextAction()`, driven by the game loop in `InvidoGfx`. Each of the three state-machine classes—`Mano`, `Giocata`, and `Partita`—has its own `std::deque` of pending actions. State-changing methods enqueue actions; each `NextAction()` processes queued work and may call back into `InvidoCore`, which can enqueue work in another state machine.

`InvidoCore::NextAction()` services the queues in this order: `Mano`, then `Giocata`, then `Partita`. This ordering allows an action processed by an earlier queue to schedule an action for a later queue during the same call (for example, a `Mano` action may request `Giocata_AMonte()`, enqueueing a `Giocata` action that can then be handled). An action enqueued for a queue that has already been serviced waits until a later `InvidoCore::NextAction()` call. `Giocata` and `Partita` each dequeue at most one action per call. `Mano` also dequeues one action normally, but its `MANO_CHANGESCORE` and `MANO_NO_ACTION` branches recursively call `NextAction()`, so one call can process additional queued actions.

When changing progression logic, preserve queue ownership and ordering, check which callbacks enqueue downstream actions, and consider whether an action runs during the current tick or the next. Avoid assuming that a single `InvidoCore::NextAction()` drains every queue completely.

## Workflow

1. Identify whether the change belongs to shared app code or `app/jni/src/GameGfx/Invido`; keep game-specific behavior in the Invido sources. Invido game-core files (`AlgCoreInterface`, `AlgPlayer`, `CardSpec`, `InvidoCoreDef`, `InvidoTraits`, `Mazzo`, `Player`, `PlayersOnTable`, `PlayerStatistic`, `Probability`, `Giocata`, `Partita`, `Mano`, `InvidoCore`, and `MatchStateWithScore`) live in `app/jni/src/GameGfx/Invido/Core`.
2. Before changing a game-flow method, callback, trait table, or interface, trace the full connection from callback definition through delegator setup and `Set...CB` wiring to the invocation site. Also follow related state transitions across `InvidoCore`, `Mano`, `Giocata`, and `Partita`.
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
