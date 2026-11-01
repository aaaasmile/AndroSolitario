# Invido Game Action Recorder — Feature Plan

## Goal

Add a `GameActionRecorder` that observes an Invido match, stores executed actions in memory in order, and writes the match and action history as JSON. The initial JSON is created at the end of `InvidoCore::Init`; the action history is flushed when the match terminates.

## JSON implementation

- Add a self-contained C/C++ JSON implementation, with source and headers, under `app/jni/src/Lib/Json`.
- Provide JSON value types for objects, arrays, strings, numbers, booleans, and null, plus parsing and writing functionality.
- Escape strings correctly when writing JSON and report malformed input clearly when parsing.
- Avoid adding an external dependency.
- Integrate the JSON source into the app build in `app/jni/src/CMakeLists.txt` and the relevant unit-test build in `app/jni/src/GameGfx/Invido/tests/CMakeLists.txt`.
- Add focused parser and writer tests covering supported value types, escaping, and malformed input.

## GameActionRecorder

- Add `GameActionRecorder.h/.cpp` under `app/jni/src/GameGfx/Invido/Core`.
- Keep an ordered `std::vector` of recorded action entries in memory.
- Use the JSON implementation to create and serialize the match document.
- Construct a unique recording filename using `GameSettings::GetSettings()->SettingsDir` and `GameName`, following the platform-aware file-location pattern in `HighScore::Save`.
- Use SDL IO functions such as `SDL_IOFromFile`, `SDL_WriteIO`, and `SDL_CloseIO` for file access.
- Recorder operations return `LPErrInApp`: return `NULL` on success and use `ERR_UTIL::ErrorCreate(...)` for failures. `InvidoCore` logs any recorder error and does not propagate it outside.

## Initial match snapshot

At the end of `InvidoCore::Init`, initialize the recorder with:

- `Mazzo`: the initial deck as a JSON array of card indexes, captured before any shuffle or deal. Add a read-only snapshot accessor to `Mazzo` if necessary.
- `Players`: an array of player objects containing each player's name and index, obtained through `PlayersOnTable` and `Player`.
- `Actions`: an initially empty array.

Use valid JSON keys and syntax (for example, `"Ix"` as a quoted key, with no trailing commas). Ensure player names are escaped by the JSON writer.

## Recording actions

- Record accepted player commands only after Invido accepts them; do not record rejected commands. Include `Say`, `PlayCard`, `VaDentro`, and abandonment.
- Record internal engine/state-machine actions when they are dequeued and executed in `Mano::NextAction`, `Giocata::NextAction`, and `Partita::NextAction`. Route notifications through the existing `InvidoCore` callback/delegator pattern rather than coupling those classes directly to the recorder.
- Include enough structured information to identify sequence/order, event source, action name, and relevant fields such as player index, card index, say value, or score.
- To support deterministic replay, also capture the shuffled deck/deal order when a `Giocata` starts; the initial unshuffled `Mazzo` list alone cannot reproduce a match.

## File update lifecycle

1. At the end of `InvidoCore::Init`, write the initial metadata document, including an empty `Actions` array.
2. Keep newly recorded actions in memory during play; do not rewrite the JSON file after every event.
3. On match termination, serialize the complete document and flush the action history to the same recording file.
4. Also finalize and flush on abandonment, which currently follows a distinct path from normal `PartitaEnded` completion.
5. Make finalization safe to call once only, avoiding duplicate final writes.

With this flush schedule, actions remain in memory until termination; a crash during play may leave only the initial JSON metadata file.

## Tests and verification

- Test the JSON parser/writer for supported values, correct string escaping, and malformed input handling.
- Test recorder metadata, action ordering, relevant event fields, and serialization.
- Test that rejected commands are not recorded and accepted commands are recorded.
- Test both initial-file creation and final flush behavior, using a temporary path or injectable IO target for tests where practical.
- Build the Invido app and tests, run the focused JSON/recorder tests, and run the Invido test executable. Report existing unrelated test failures separately.
