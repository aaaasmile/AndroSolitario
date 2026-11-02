# GameActionReplayer — Replay Feature Plan

## Goal

Add `GameActionReplayer` to load a completed JSON match produced by `GameActionRecorder` and replay the recorded game through Invido's existing player-algorithm and core-game interfaces.

## New classes and responsibilities

### `GameActionReplayer`

- Add `GameActionReplayer.h/.cpp` under `app/jni/src/GameGfx/Invido/Core`.
- Load a recorder JSON file using the bundled parser in `app/jni/src/Lib/Json`.
- Validate the document version, `Players`, and ordered `Actions`; report malformed or unsupported recordings with `LPErrInApp` errors.
- Maintain a shared playback cursor over the action list. Coordinate both player algorithms so each consumes the next relevant player input for its player without skipping or reordering player inputs belonging to the other player. Non-player events are skipped by the replay reader.
- Provide player metadata (names/indexes) to initialize replay players from the recording.

### `AlgPlayerAuto`

- Add `AlgPlayerAuto.h/.cpp` under `app/jni/src/GameGfx/Invido/Core`, implementing `I_ALG_Player` like `AlgPlayer`.
- `Player::InitFromActionPlayer(GameActionReplayer&, Uint8 playerIx)` creates and owns an `AlgPlayerAuto` configured for that player index and the shared action player.
- Implement all required `I_ALG_Player` methods. For now, the replayer is only used by unit tests; do not add a replay UI or integrate replay setup into Invido graphics.
- `ALG_HaveToPlay()` and `ALG_HaveToRespond()` request the next relevant player-input action from `GameActionReplayer` and invoke the matching `I_CORE_Game` method (`Say` or `PlayCard`). `MANO_SAYBUIDA` is an InvidoCore notification about the correctness of a submitted `Say`, not a recorded player action for the replay algorithm.
- `AlgPlayerAuto` consumes only recorder entries whose `Action` is `PlayCard` or `Say`. All other actions—including `VaDentro`, `Abandon`, `Deal`, `ShuffledDeck`, and state-machine events—are irrelevant to `AlgPlayerAuto` and must be skipped without being treated as commands.
- Use the recorded `Player`, `Say`, and `Card` fields to reconstruct these calls. Resolve card indexes to `CardSpec` before invoking card APIs.
- Never consume `PlayCard` or `Say` entries for the other player. If the next relevant entry cannot be performed in the current callback/state, return/report a clear replay error rather than silently advancing.

## Recorder format and replay compatibility

- Use the recorder's `Players` metadata to rebuild the players through `InitFromActionPlayer`, retaining the recorded indexes and names.
- Parse the `Engine` `MatchStarted` action's `Player` field as the match's first player. In replay mode, `InvidoCore::NewMatch()` must use that index instead of choosing a random first player.
- Parse each `Engine` `ShuffledDeck` action's `Mazzo` array, pairing it with the immediately preceding `Giocata` `Started` player and preserving deck snapshots in order. Add a validated deck-order setter to `Mazzo`. In replay mode, `InvidoCore::Giocata_Start()` consumes the next recorded shuffled order and installs it instead of calling `Shuffle()`; this makes the subsequent deal deterministic.
- `AlgPlayerAuto` consumes only recorder entries whose source is `Player` and whose action is `Say` or `PlayCard`. `MatchStarted` and `ShuffledDeck` are replayer/core setup data, not algorithm commands. `VaDentro`, `Abandon`, `Deal`, and state-machine events are ignored by `AlgPlayerAuto`.
- `ALG_HaveToRespond()` executes only `Say`. `ALG_HaveToPlay()` may execute the next recorded `Say` or `PlayCard`: existing recordings can contain a call during a turn notified as “to play.” `InvidoCore::NtyWaitingPlayer_Toplay()` inspects the next recorded action and routes a `Say` to `ALG_HaveToRespond()`; otherwise it routes to `ALG_HaveToPlay()`. A response callback receiving a non-`Say` action is reported without consuming the event.
- Preserve the existing live `AlgPlayer` / `Player::Init` path. Add `InvidoCore::SetGameActionReplayer(GameActionReplayer&)` for explicit replay mode; normal games continue using a null replayer and the existing random shuffle/start-player logic.
- Keep the replayer and `AlgPlayerAuto` available to unit tests only for this phase; update `testReplayMatch` to set the replayer directly. Do not add UI or production graphics integration.

## Integration points

1. Add `Player::InitFromActionPlayer(GameActionReplayer&, Uint8 playerIx)` and ensure the new `AlgPlayerAuto` is owned and released by `Player::CleanUp()` / destruction. Preserve the existing `Player::Init` path.
2. Keep `InvidoCore` responsible for dispatching through the existing `I_ALG_Player` interface. Avoid coupling the core state machines directly to `GameActionReplayer`.
3. Keep replay initialization and execution confined to unit tests for now. Do not modify Invido graphics or add a user-facing replay flow.
4. Integrate new sources into the app and test CMake files as required by the build; do not add production graphics setup or UI.
5. Keep errors in the project's `LPErrInApp` convention. Respect `ErrorCreate`'s 10-slot rotating static storage: do not delete returned pointers and copy/use messages before more than ten subsequent calls.

## Tests

- JSON loading: valid recording, unsupported format version, missing keys, wrong value types, malformed JSON, and escaped names.
- Cursor and turn behavior: interleaved `Say` and `PlayCard` entries for two players; verify one player's callback never consumes the other's next relevant action and irrelevant entries are skipped.
- Action dispatch: `ALG_HaveToPlay()` can replay a recorded `Say` or `PlayCard`, while `ALG_HaveToRespond()` only replays `Say`; a response mismatch does not consume input. Confirm unrelated events are never sent to `I_CORE_Game` by `AlgPlayerAuto`.
- Replay setup: `MatchStarted.Player` controls the first player; consecutive `ShuffledDeck.Mazzo` snapshots override each shuffled deck in order.
- Player lifecycle: `InitFromActionPlayer` creates the intended algorithm; cleanup frees owned replay algorithms once and does not free externally owned/shared `GameActionReplayer` state.
- End-to-end replay: replay the checked-in two-player match recording to completion using only unit-test setup, without graphics integration.

## Recorder compatibility

The recorder also logs engine/state-machine events; `MANO_SAYBUIDA` is the engine notification for a rejected `Say`, not a player input. Replay dispatch is defined by the recorded `Action` field and `AlgPlayerAuto` consumes only `Say` and `PlayCard` records. Other event types are ignored by that algorithm.
