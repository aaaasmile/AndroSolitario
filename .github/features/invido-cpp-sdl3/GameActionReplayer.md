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
- `ALG_HaveToPlay()` and `ALG_HaveToRespond()` request the next relevant player-input action from `GameActionReplayer` and invoke only the corresponding `I_CORE_Game` method: `Say`, `PlayCard`, or `VaDentro`. `MANO_SAYBUIDA` is an InvidoCore notification about the correctness of a submitted `Say`, not a recorded player action for the replay algorithm.
- `AlgPlayerAuto` consumes only recorder entries whose `Action` is `PlayCard` or `Say`. All other actions—including `VaDentro`, `Abandon`, `Deal`, `ShuffledDeck`, and state-machine events—are irrelevant to `AlgPlayerAuto` and must be skipped without being treated as commands.
- Use the recorded `Player`, `Say`, and `Card` fields to reconstruct these calls. Resolve card indexes to `CardSpec` before invoking card APIs.
- Never consume `PlayCard` or `Say` entries for the other player. If the next relevant entry cannot be performed in the current callback/state, return/report a clear replay error rather than silently advancing.

## Recorder format and replay compatibility

- Use the recorder's `Players` metadata to rebuild the two players through `InitFromActionPlayer`, retaining the recorded indexes and names.
- `AlgPlayerAuto` ignores deal and state-machine entries, so it does not need to use `Deal` records as player inputs. If replay setup requires card snapshots to keep `InvidoCore` and scoring state consistent, handle that in test/replay initialization or a separate replay coordinator rather than dispatching those events from `AlgPlayerAuto`.
- Preserve the existing live `AlgPlayer` / `Player::Init` path. `InitFromActionPlayer` is the explicit replay initialization path; normal games remain unchanged.
- Keep the replayer and `AlgPlayerAuto` available to unit tests only for this phase; do not add UI or production graphics integration.

## Integration points

1. Add `Player::InitFromActionPlayer(GameActionReplayer&, Uint8 playerIx)` and ensure the new `AlgPlayerAuto` is owned and released by `Player::CleanUp()` / destruction. Preserve the existing `Player::Init` path.
2. Keep `InvidoCore` responsible for dispatching through the existing `I_ALG_Player` interface. Avoid coupling the core state machines directly to `GameActionReplayer`.
3. Keep replay initialization and execution confined to unit tests for now. Do not modify Invido graphics or add a user-facing replay flow.
4. Integrate new sources into the app and test CMake files as required by the build; do not add production graphics setup or UI.
5. Keep errors in the project's `LPErrInApp` convention. Respect `ErrorCreate`'s 10-slot rotating static storage: do not delete returned pointers and copy/use messages before more than ten subsequent calls.

## Tests

- JSON loading: valid recording, unsupported format version, missing keys, wrong value types, malformed JSON, and escaped names.
- Cursor and turn behavior: interleaved `Say` and `PlayCard` entries for two players; verify one player's callback never consumes the other's next relevant action and irrelevant entries are skipped.
- Action dispatch: `Say` and `PlayCard` entries reconstruct the expected core calls. Confirm `VaDentro`, `Abandon`, `Deal`, `ShuffledDeck`, and state-machine events are ignored by `AlgPlayerAuto`.
- Player lifecycle: `InitFromActionPlayer` creates the intended algorithm; cleanup frees owned replay algorithms once and does not free externally owned/shared `GameActionReplayer` state.
- Unit-test replay: exercise the replay path without integrating it into Invido graphics or adding a user-facing flow.

## Recorder compatibility

The recorder also logs engine/state-machine events; `MANO_SAYBUIDA` is the engine notification for a rejected `Say`, not a player input. Replay dispatch is defined by the recorded `Action` field and `AlgPlayerAuto` consumes only `Say` and `PlayCard` records. Other event types are ignored by that algorithm.
