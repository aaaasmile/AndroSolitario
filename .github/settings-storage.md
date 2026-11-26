# Game settings storage and persistence

`GameSettings` is a process-wide settings object returned by `GameSettings::GetSettings()`. A game-specific `GameSelector` sets its game type and name with `SetGameTypeGfx(...)` (currently `"Invido"` or `"Solitario"`). That name identifies the settings file; the persistence functions themselves are shared.

## Startup and loading

`AppGfx::Init()` initializes game-specific defaults with `GameSelector::InitializeGameSettings()`, then calls `loadProfile()`. `loadProfile()` creates the per-user settings directory, sets `GameSettings::SettingsDir`, calls `GameSettings::LoadSettings()`, and finally invokes `GameSelector::ApplyLoadedSettings()` to validate or normalize game-specific values. The selected language is applied to the `Languages` manager after profile loading.

`LoadSettings()` uses the same settings file selected by `setSettingFileName()` on native platforms:

```text
<SettingsDir>/<GameName>.bin
```

The profile directory is platform-specific: Windows uses `%HOMEDRIVE%%HOMEPATH%/.<GameName><VERSION_HOME>`, Linux uses `$HOME/.<GameName><VERSION_HOME>`, and Android uses the internal storage path. `VERSION_HOME` currently versions the profile directory. For example, the Windows Invido profile uses `Invido.bin` inside `.Invido030007`. Emscripten builds instead read the corresponding keys from browser `localStorage`.

### Native binary format

The native file starts with the common settings, in this order:

| Field | Encoding |
| --- | --- |
| Deck index | 1 byte |
| Language ID | 1 byte |
| Music enabled | 1 byte (`0` or `1`) |
| Legacy background type | 1 byte |
| Player-name length | 1 byte |
| Player name | UTF-8 bytes, as many as the preceding length |

The common section is followed by the `GOPT` extension marker, a 1-byte extension version, and a 2-byte option count. Each game-specific option is then serialized as a 1-byte key length, 2-byte value length, key bytes, and value bytes. The extension ends with the current background type in 1 byte. The current writer uses version `1` and writes the `GameOptions` map entries in key order.

The loader reads the common fields, recognizes the extension, and rebuilds `GameOptions` from its key/value entries. Older files without the extension are migrated from their legacy background field and saved in the current format. Files produced by the earlier extension writer that omitted its final background byte are also accepted, using the legacy background field as fallback.

## Game-specific options use the same save/load functions

Game-specific settings are entries in the shared `std::map<std::string, std::string> GameOptions`, not a separate file or persistence API. A game defines stable keys, inserts defaults before loading, validates loaded values afterward, and changes the map when the player edits an option. The shared `SaveSettings()` writes the whole map alongside the common settings, and `LoadSettings()` restores all serialized entries into the same map.

For example, Solitario uses `GAMESET::SceneBackgroundOptionKey` (`"SceneBackground"`) to store values such as `"Commessaggio"` and `"Mantova"`. Its selector seeds a default before `LoadSettings()` and validates the value after load; the options UI updates the map, then calls `SaveSettings()` when settings change. A game-specific option follows the same pattern:

```cpp
settings->GameOptions["OptionKey"] = "default";  // before LoadSettings
// LoadSettings() restores saved entries; ApplyLoadedSettings() validates them.
settings->GameOptions["OptionKey"] = selectedValue;
settings->SaveSettings();
```

On Emscripten, the same public `LoadSettings()` and `SaveSettings()` functions store each map entry under `GameOption_<key>` in localStorage, alongside the common setting keys.

`SaveGameOptions()` is a convenience wrapper for the game-options subset, but for native builds it delegates to `SaveSettings()`. Updating a value in `GameOptions` alone changes only in-memory state; the change persists after restart only once `SaveSettings()` (or `SaveGameOptions()`) completes successfully.

## Format and maintenance notes

- Keep option keys stable; changing a key means adding a migration or treating it as a new setting.
- Add defaults before `LoadSettings()` so the game can use a default when an option has never been saved.
- Validate or migrate option values in the game's `ApplyLoadedSettings()` implementation.
- `SaveSettings()` returns `LPErrInApp`; callers should propagate or log errors rather than assuming the write succeeded.
- When changing the native binary layout, keep read/write order aligned and bump the extension version for incompatible changes.
