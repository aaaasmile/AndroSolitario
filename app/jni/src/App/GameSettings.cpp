#include "GameSettings.h"

#include <SDL3/SDL.h>
#include <limits>
#include <string.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "Config.h"
#include "MusicManager.h"

#if PLATFORM_EMS
#include <emscripten.h>
#include <emscripten/bind.h>

EMSCRIPTEN_KEEPALIVE
void setLanguage(const std::string& languageCode) {
    TRACE("[setLanguage] Language changed to: %s\n", languageCode.c_str());
    LPGameSettings pGameSetting = GameSettings::GetSettings();
    bool has_changed = false;
    if (languageCode.compare("it") == 0) {
        has_changed =
            pGameSetting->CurrentLanguage != Languages::eLangId::LANG_ITA;
        pGameSetting->CurrentLanguage = Languages::eLangId::LANG_ITA;
    } else if (languageCode.compare("en") == 0) {
        has_changed =
            pGameSetting->CurrentLanguage != Languages::eLangId::LANG_ENG;
        pGameSetting->CurrentLanguage = Languages::eLangId::LANG_ENG;
    }
    if (has_changed) {
        pGameSetting->SetCurrentLang();
        pGameSetting->SaveSettings();
    }
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("setLanguage", &setLanguage);
}

#endif

static LPGameSettings _p_GameSettings = NULL;
static char _settingsRootDir[1024] = "";
static char _exeRootDir[1024] = "";
static char g_filepath[1024];

static const char* g_lpszIniFontAriblkFname =
    DATA_PREFIX "font/DejaVuSans-Bold.ttf";
static const char* g_lpszFontSymbFname = DATA_PREFIX "font/notosans-sym.ttf";
static const char* g_lpszFontDejFname = DATA_PREFIX "font/DejaVuSans.ttf";

static const char g_lpszSettingsExtension[] = "GOPT";
static const uint8_t g_settingsExtensionVersion = 1;

static LPErrInApp readSettingsData(SDL_IOStream* src, void* data,
                                   size_t size) {
    if (SDL_ReadIO(src, data, size) != size) {
        return ERR_UTIL::ErrorCreate("SDL_ReadIO error %s\n", SDL_GetError());
    }
    return NULL;
}

static LPErrInApp writeSettingsData(SDL_IOStream* dst, const void* data,
                                    size_t size) {
    if (SDL_WriteIO(dst, data, size) != size) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }
    return NULL;
}

static LPErrInApp migrateLegacyBackground(GameSettings* settings,
                                          uint8_t legacyBackgroundType) {
    switch (legacyBackgroundType) {
        case 0:
            settings->BackgroundType = BackgroundTypeEnum::Image;
            settings->GameOptions[GAMESET::SceneBackgroundOptionKey] =
                "Commessaggio";
            break;
        case 1:
            settings->BackgroundType = BackgroundTypeEnum::Image;
            settings->GameOptions[GAMESET::SceneBackgroundOptionKey] =
                "Mantova";
            break;
        case 2:
            settings->BackgroundType = BackgroundTypeEnum::Black;
            break;
        default:
            return ERR_UTIL::ErrorCreate(
                "Background type %d in settings is not supported\n",
                legacyBackgroundType);
    }
    return NULL;
}

LPGameSettings GameSettings::GetSettings() {
    if (_p_GameSettings == NULL) {
        _p_GameSettings = new GameSettings();
    }
    return _p_GameSettings;
}

GameSettings::GameSettings() {
    // default settings
    PlayerName = "Virginio";
    DeckTypeVal.SetType(eDeckType::PIACENTINA);
    CurrentLanguage = Languages::LANG_ENG;
    MusicEnabled = true;
    SettingsDir = "";
    BackgroundType = BackgroundTypeEnum::Image;
    _p_Languages = NULL;
    _p_MusicManager = NULL;
    _p_fontDejUnderscoreSmall = NULL;
    GameName = "";
}

GameSettings::~GameSettings() {
    if (_p_Languages) {
        delete _p_Languages;
        _p_Languages = NULL;
    }
}

void GameSettings::SetCurrentLang() {
    GetLanguageMan();
    _p_Languages->SetLang(CurrentLanguage);
}

Languages* GameSettings::GetLanguageMan() {
    if (_p_Languages == NULL) {
        _p_Languages = new Languages();
    }
    return _p_Languages;
}

LPErrInApp GameSettings::InitMusicManager() {
    if (_p_MusicManager == NULL) {
        _p_MusicManager = new MusicManager();
    } else {
        return ERR_UTIL::ErrorCreate("Sound manager already initialized");
    }
    LPErrInApp err = _p_MusicManager->Initialize(MusicEnabled);
    if (err != NULL) {
        TRACE("Music System error: %s\n", err->ErrorText.c_str());
    }
    return NULL;
}

void GameSettings::TerminateMusicManager() {
    if (_p_MusicManager) {
        _p_MusicManager->Terminate();
        delete _p_MusicManager;
        _p_MusicManager = NULL;
    }
}

#if PLATFORM_EMS
static std::string loadGameSetting(const char* key, const char* defaultValue) {
    char js_command[256];
    snprintf(js_command, sizeof(js_command),
             "localStorage.getItem('game_%s') || '%s'", key, defaultValue);
    const char* result = emscripten_run_script_string(js_command);
    return result != NULL ? std::string(result) : std::string(defaultValue);
}

static void saveGameSetting(const char* key, const char* value) {
    char js_cmd[256];
    snprintf(js_cmd, sizeof(js_cmd), "localStorage.setItem('game_%s', '%s')",
             key, value);
    emscripten_run_script(js_cmd);
}
#endif

LPErrInApp GameSettings::LoadSettings() {
#if PLATFORM_EMS
    std::string defval = MusicEnabled ? "true" : "false";
    MusicEnabled = loadGameSetting("MusicEnabled", defval.c_str()) == "true";

    defval = std::to_string(CurrentLanguage);
    CurrentLanguage = static_cast<Languages::eLangId>(
        std::stoi(loadGameSetting("CurrentLanguage", defval.c_str())));

    defval = std::to_string(DeckTypeVal.GetTypeIndex());
    DeckTypeVal.SetTypeIndex(
        std::stoi(loadGameSetting("DeckTypeVal", defval.c_str())));

    for (std::map<std::string, std::string>::iterator option =
             GameOptions.begin();
         option != GameOptions.end(); ++option) {
        const std::string storageKey = "GameOption_" + option->first;
        option->second = loadGameSetting(storageKey.c_str(),
                                         option->second.c_str());
    }

    const std::string storedBackground =
        loadGameSetting("BackgroundType", "Image");
    bool migratedLegacyBackground = false;
    if (storedBackground == "Black") {
        BackgroundType = BackgroundTypeEnum::Black;
    } else if (storedBackground == "Image") {
        BackgroundType = BackgroundTypeEnum::Image;
    } else if (storedBackground == "0" || storedBackground == "1" ||
               storedBackground == "2") {
        LPErrInApp err = migrateLegacyBackground(
            this, static_cast<uint8_t>(storedBackground[0] - '0'));
        if (err != NULL) {
            return err;
        }
        migratedLegacyBackground = true;
    } else {
        return ERR_UTIL::ErrorCreate("Background setting '%s' is invalid\n",
                                     storedBackground.c_str());
    }

    PlayerName = loadGameSetting("PlayerName", PlayerName.c_str());
    if (migratedLegacyBackground) {
        return SaveSettings();
    }
    return NULL;
#else
    LPErrInApp err = setSettingFileName();
    if (err != NULL) {
        return err;
    }

    TRACE("Load setting file %s\n", g_filepath);
    SDL_IOStream* src = SDL_IOFromFile(g_filepath, "rb");
    if (src == NULL) {
        TRACE("No setting file found, no problem ignore it and use default\n");
        return NULL;
    }

    uint8_t deckId;
    uint8_t langId;
    uint8_t musEnabled;
    uint8_t legacyBackgroundType;
    uint8_t nameSize;
    err = readSettingsData(src, &deckId, sizeof(deckId));
    if (err == NULL) {
        err = readSettingsData(src, &langId, sizeof(langId));
    }
    if (err == NULL) {
        err = readSettingsData(src, &musEnabled, sizeof(musEnabled));
    }
    if (err == NULL) {
        err = readSettingsData(src, &legacyBackgroundType,
                               sizeof(legacyBackgroundType));
    }
    if (err == NULL) {
        err = readSettingsData(src, &nameSize, sizeof(nameSize));
    }
    if (err != NULL) {
        SDL_CloseIO(src);
        return err;
    }

    std::string playerName(nameSize, '\0');
    if (nameSize > 0) {
        err = readSettingsData(src, &playerName[0], nameSize);
        if (err != NULL) {
            SDL_CloseIO(src);
            return err;
        }
    }

    const Sint64 extensionOffset = SDL_TellIO(src);
    if (extensionOffset < 0) {
        SDL_CloseIO(src);
        return ERR_UTIL::ErrorCreate("Unable to locate settings extension");
    }

    const size_t extensionMagicSize = sizeof(g_lpszSettingsExtension) - 1;
    char extensionMagic[sizeof(g_lpszSettingsExtension) - 1];
    const size_t bytesRead = SDL_ReadIO(src, extensionMagic,
                                        extensionMagicSize);
    const bool hasExtension =
        bytesRead == extensionMagicSize &&
        memcmp(extensionMagic, g_lpszSettingsExtension, extensionMagicSize) ==
            0;
    uint8_t extensionVersion = 0;
    if (hasExtension) {
        err = readSettingsData(src, &extensionVersion,
                               sizeof(extensionVersion));
        if (err != NULL) {
            SDL_CloseIO(src);
            return err;
        }
        if (extensionVersion != g_settingsExtensionVersion) {
            SDL_CloseIO(src);
            return ERR_UTIL::ErrorCreate(
                "Settings extension version %d is not supported\n",
                extensionVersion);
        }

        uint16_t optionCount;
        err = readSettingsData(src, &optionCount, sizeof(optionCount));
        if (err != NULL) {
            SDL_CloseIO(src);
            return err;
        }
        for (uint16_t index = 0; index < optionCount; ++index) {
            uint8_t keySize;
            uint16_t valueSize;
            err = readSettingsData(src, &keySize, sizeof(keySize));
            if (err == NULL) {
                err = readSettingsData(src, &valueSize, sizeof(valueSize));
            }
            if (err != NULL) {
                SDL_CloseIO(src);
                return err;
            }

            std::string key(keySize, '\0');
            std::string value(valueSize, '\0');
            if (keySize > 0) {
                err = readSettingsData(src, &key[0], keySize);
            }
            if (err == NULL && valueSize > 0) {
                err = readSettingsData(src, &value[0], valueSize);
            }
            if (err != NULL) {
                SDL_CloseIO(src);
                return err;
            }
            GameOptions[key] = value;
        }

        uint8_t backgroundType;
        err = readSettingsData(src, &backgroundType, sizeof(backgroundType));
        if (err != NULL) {
            SDL_CloseIO(src);
            return err;
        }
        if (backgroundType != BackgroundTypeEnum::Black &&
            backgroundType != BackgroundTypeEnum::Image) {
            SDL_CloseIO(src);
            return ERR_UTIL::ErrorCreate("Background type %d is invalid\n",
                                         backgroundType);
        }
        BackgroundType = static_cast<BackgroundTypeEnum>(backgroundType);
        SDL_CloseIO(src);
        src = NULL;
    } else {
        if (SDL_SeekIO(src, extensionOffset, SDL_IO_SEEK_SET) < 0) {
            SDL_CloseIO(src);
            return ERR_UTIL::ErrorCreate("Unable to rewind settings file");
        }
    }
    if (src != NULL) {
        SDL_CloseIO(src);
    }

    DeckTypeVal.SetTypeIndex(deckId);
    CurrentLanguage = static_cast<Languages::eLangId>(langId);
    MusicEnabled = musEnabled != 0;
    PlayerName = playerName;
    if (!hasExtension) {
        err = migrateLegacyBackground(this, legacyBackgroundType);
        if (err != NULL) {
            return err;
        }
        return SaveSettings();
    }
    return NULL;
#endif
}

LPErrInApp GameSettings::SaveSettings() {
#if PLATFORM_EMS
    std::string value = MusicEnabled ? "true" : "false";
    saveGameSetting("MusicEnabled", value.c_str());
    value = std::to_string(CurrentLanguage);
    saveGameSetting("CurrentLanguage", value.c_str());
    value = std::to_string(DeckTypeVal.GetTypeIndex());
    saveGameSetting("DeckTypeVal", value.c_str());
    value = BackgroundType == BackgroundTypeEnum::Black ? "Black" : "Image";
    saveGameSetting("BackgroundType", value.c_str());
    for (std::map<std::string, std::string>::const_iterator option =
             GameOptions.begin();
         option != GameOptions.end(); ++option) {
        const std::string storageKey = "GameOption_" + option->first;
        saveGameSetting(storageKey.c_str(), option->second.c_str());
    }
    saveGameSetting("PlayerName", PlayerName.c_str());
    return NULL;
#else
    LPErrInApp err = setSettingFileName();
    if (err != NULL) {
        return err;
    }
    if (PlayerName.size() > std::numeric_limits<uint8_t>::max()) {
        return ERR_UTIL::ErrorCreate("Player name is too long to save");
    }
    if (GameOptions.size() > std::numeric_limits<uint16_t>::max()) {
        return ERR_UTIL::ErrorCreate("Too many game settings");
    }

    const uint8_t deckId =
        static_cast<uint8_t>(DeckTypeVal.GetTypeIndex());
    const uint8_t langId = static_cast<uint8_t>(CurrentLanguage);
    const uint8_t musicEnabled = static_cast<uint8_t>(MusicEnabled);
    const uint8_t backgroundType = static_cast<uint8_t>(BackgroundType);
    const uint8_t nameSize = static_cast<uint8_t>(PlayerName.size());
    const uint16_t optionCount = static_cast<uint16_t>(GameOptions.size());

    SDL_IOStream* dst = SDL_IOFromFile(g_filepath, "wb");
    if (dst == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to save setting file %s",
                                     g_filepath);
    }

    err = writeSettingsData(dst, &deckId, sizeof(deckId));
    if (err == NULL) {
        err = writeSettingsData(dst, &langId, sizeof(langId));
    }
    if (err == NULL) {
        err = writeSettingsData(dst, &musicEnabled, sizeof(musicEnabled));
    }
    if (err == NULL) {
        err = writeSettingsData(dst, &backgroundType, sizeof(backgroundType));
    }
    if (err == NULL) {
        err = writeSettingsData(dst, &nameSize, sizeof(nameSize));
    }
    if (err == NULL && nameSize > 0) {
        err = writeSettingsData(dst, PlayerName.data(), nameSize);
    }
    if (err == NULL) {
        err = writeSettingsData(dst, g_lpszSettingsExtension,
                                sizeof(g_lpszSettingsExtension) - 1);
    }
    if (err == NULL) {
        err = writeSettingsData(dst, &g_settingsExtensionVersion,
                                sizeof(g_settingsExtensionVersion));
    }
    if (err == NULL) {
        err = writeSettingsData(dst, &optionCount, sizeof(optionCount));
    }
    for (std::map<std::string, std::string>::const_iterator option =
             GameOptions.begin();
         err == NULL && option != GameOptions.end(); ++option) {
        if (option->first.size() > std::numeric_limits<uint8_t>::max() ||
            option->second.size() > std::numeric_limits<uint16_t>::max()) {
            SDL_CloseIO(dst);
            return ERR_UTIL::ErrorCreate("Game setting is too long to save");
        }

        const uint8_t keySize = static_cast<uint8_t>(option->first.size());
        const uint16_t valueSize =
            static_cast<uint16_t>(option->second.size());
        err = writeSettingsData(dst, &keySize, sizeof(keySize));
        if (err == NULL) {
            err = writeSettingsData(dst, &valueSize, sizeof(valueSize));
        }
        if (err == NULL && keySize > 0) {
            err = writeSettingsData(dst, option->first.data(), keySize);
        }
        if (err == NULL && valueSize > 0) {
            err = writeSettingsData(dst, option->second.data(), valueSize);
        }
    }
    const bool closed = SDL_CloseIO(dst);
    if (!closed && err == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to close settings file: %s\n",
                                     SDL_GetError());
    }
    return err;
#endif
}

LPErrInApp GameSettings::SaveGameOptions() {
#if PLATFORM_EMS
    std::string value = BackgroundType == BackgroundTypeEnum::Black ? "Black" : "Image";
    saveGameSetting("BackgroundType", value.c_str());
    for (std::map<std::string, std::string>::const_iterator option = GameOptions.begin();
         option != GameOptions.end(); ++option) {
        const std::string storageKey = "GameOption_" + option->first;
        saveGameSetting(storageKey.c_str(), option->second.c_str());
    }
    return NULL;
#else
    return SaveSettings();
#endif
}
void GameSettings::SetGameTypeGfx(enum GameTypeEnum gmType) {
    _gameType = gmType;
    switch (gmType) {
        case GameTypeEnum::Invido:
            GameName = "Invido";
            break;
        case GameTypeEnum::Solitario:
            GameName = "Solitario";
            break;

        default:
            break;
    }
}

LPErrInApp GameSettings::LoadFonts() {
    TRACE_DEBUG("GameSettings::LoadFonts %s \n", g_lpszFontDejFname);

    _p_fontDejBoldBig = TTF_OpenFont(g_lpszIniFontAriblkFname, _fontBigSize);
    if (_p_fontDejBoldBig == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszIniFontAriblkFname, SDL_GetError());
    }
    _p_fontDejBig = TTF_OpenFont(g_lpszFontDejFname, _fontBigSize);
    if (_p_fontDejBig == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszFontDejFname, SDL_GetError());
    }
    TRACE_DEBUG("LoadFonts Ariblk OK \n");
    _p_fontDejSmall = TTF_OpenFont(g_lpszFontDejFname, _fontSmallSize);
    if (_p_fontDejSmall == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszFontDejFname, SDL_GetError());
    }
    _p_fontMedium = TTF_OpenFont(g_lpszFontDejFname, _fontMediumSize);
    if (_p_fontMedium == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Unable to load medium font %s, error: %s\n", g_lpszFontDejFname,
            SDL_GetError());
    }
    _p_fontSymb = TTF_OpenFont(g_lpszFontSymbFname, _fontSymSize);
    if (_p_fontSymb == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszFontSymbFname, SDL_GetError());
    }
    _p_fontDejMedium = TTF_OpenFont(g_lpszFontDejFname, _fontMediumSize);
    if (_p_fontDejMedium == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszFontDejFname, SDL_GetError());
    }

    _p_fontDejUnderscoreSmall =
        TTF_OpenFont(g_lpszFontDejFname, _fontSmallSize);
    if (_p_fontDejUnderscoreSmall == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszFontDejFname, SDL_GetError());
    }
    TTF_SetFontStyle(_p_fontDejUnderscoreSmall, TTF_STYLE_UNDERLINE);
    return NULL;
}

LPErrInApp GameSettings::setSettingFileName() {
    if (SettingsDir == "" || GameName == "") {
        return ERR_UTIL::ErrorCreate("User dir for setting is not defined");
    }
    snprintf(g_filepath, sizeof(g_filepath), "%s/%s.bin", SettingsDir.c_str(),
             GameName.c_str());
    return NULL;
}

void GameSettings::GetTouchPoint(SDL_TouchFingerEvent& tfinger,
                                 SDL_Point* pPoint) {
    // TRACE_DEBUG("tap on %f, %f - w/h %d/%d\n", tfinger.x, tfinger.y,
    //             _screenRect.w, _screenRect.h);
    pPoint->x = tfinger.x * _screenRect.w;
    pPoint->y = tfinger.y * _screenRect.h;
}

void GameSettings::SetDisplaySize(int w, int h) {
    _screenRect.w = w;
    _screenRect.h = h;
}

//  Namespace GAMESET

void GAMESET::GetNameWithAssets(const char* src_path, std::string& res) {
#ifdef ANDROID
    char fpath[PATH_MAX];
    snprintf(fpath, sizeof(fpath), "%s/%s", SDL_GetAndroidInternalStoragePath(),
             src_path);
    res = fpath;
#else
    res = src_path;
#endif
    TRACE_DEBUG("[GetNameWithAssets] : %s\n", res.c_str());
}

const char* GAMESET::GetExeAppFolder() {
    if (strlen(_exeRootDir) > 0) {
        return _exeRootDir;
    }

    const char* path = SDL_GetBasePath();

    TRACE("Exe path: %s\n", path);
    snprintf(_exeRootDir, sizeof(_exeRootDir), "%s", path);

    TRACE("RootDir: %s\n", _exeRootDir);

    return _exeRootDir;
}

const char* GAMESET::GetHomeFolder() {
    if (strlen(_settingsRootDir) > 0) {
        return _settingsRootDir;
    }
#ifdef ANDROID
    sprintf(_settingsRootDir, "%s/.solitario%s",
            SDL_GetAndroidInternalStoragePath(), VERSION_HOME);
    return _settingsRootDir;
#endif
#ifdef WIN32
    sprintf(_settingsRootDir, "%s/%s/.solitario%s", getenv("HOMEDRIVE"),
            getenv("HOMEPATH"), VERSION_HOME);
#else
    sprintf(_settingsRootDir, "%s/.solitario%s", getenv("HOME"), VERSION_HOME);
#endif
    return _settingsRootDir;
}

LPErrInApp GAMESET::CreateHomeFolderIfNotExists(bool& dirCreated) {
    dirCreated = false;
#if PLATFORM_EMS
    return NULL;
#endif
    struct stat st = {0};
    int io_res;
    GetHomeFolder();
    if (stat(_settingsRootDir, &st) == -1) {
#ifdef WIN32
        io_res = mkdir(_settingsRootDir);
#else
        io_res = mkdir(_settingsRootDir, 0700);
#endif
        if (io_res == -1) {
            return ERR_UTIL::ErrorCreate("Cannot create dir %s",
                                         _settingsRootDir);
        } else {
            dirCreated = true;
        }
    }
    return NULL;
}
