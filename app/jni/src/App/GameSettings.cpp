#include "GameSettings.h"

#include <SDL3/SDL.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include "Config.h"
#include "MusicManager.h"
#include "WinTypeGlobal.h"

#if PLATFORM_EMS
#include <emscripten.h>
#endif

static LPGameSettings _p_GameSettings = NULL;
static char _settingsRootDir[1024] = "";
static char _exeRootDir[1024] = "";
static char g_filepath[1024];

static const char* g_lpszIniFontAriblkFname = DATA_PREFIX "font/ariblk.ttf";
static const char* g_lpszIniFontVeraFname = DATA_PREFIX "font/vera.ttf";
static const char* g_lpszFontSymbFname = DATA_PREFIX "font/notosans-sym.ttf";

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
    BackgroundType = BackgroundTypeEnum::Mantova;
    _p_Languages = NULL;
    _p_MusicManager = NULL;
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
    std::string result = loadGameSetting("MusicEnabled", defval.c_str());
    //TRACE_DEBUG("Read setting result '%s'\n", result.c_str());
    MusicEnabled = result == "true" ? true : false;

    defval = std::to_string(CurrentLanguage);
    result = loadGameSetting("CurrentLanguage", defval.c_str());
    int langId = std::stoi(result);    
    CurrentLanguage = (Languages::eLangId)langId;

    defval = std::to_string(DeckTypeVal.GetTypeIndex());
    result = loadGameSetting("DeckTypeVal", defval.c_str());
    int deckId = std::stoi(result);
    DeckTypeVal.SetTypeIndex(deckId);

    defval = std::to_string(BackgroundType);
    result = loadGameSetting("BackgroundType", defval.c_str());
    int bt = std::stoi(result);
    BackgroundType = (BackgroundTypeEnum)bt;

    defval = PlayerName;
    PlayerName = loadGameSetting("PlayerName", defval.c_str());
    
#else
    LPErrInApp err = setSettingFileName();
    if (err != NULL) {
        return err;
    }

    TRACE("Load setting file %s\n", g_filepath);
    SDL_IOStream* src = SDL_IOFromFile(g_filepath, "rb");
    if (src == 0) {
        TRACE("No setting file found, no problem ignore it and use default\n");
        return NULL;
    }

    uint8_t deckId;
    uint8_t langId;
    uint8_t musEnabled;
    uint8_t backgroudType;
    uint8_t nameSize;

    if (SDL_ReadIO(src, &deckId, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    if (SDL_ReadIO(src, &langId, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    if (SDL_ReadIO(src, &musEnabled, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    if (SDL_ReadIO(src, &backgroudType, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    // player name
    if (SDL_ReadIO(src, &nameSize, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    std::string playername;
    if (nameSize > 0) {
        playername.resize(nameSize);
        if (SDL_ReadIO(src, (void*)playername.data(), nameSize) == 0) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
                SDL_GetError());
        }
    }

    SDL_CloseIO(src);

    DeckTypeVal.SetTypeIndex(deckId);
    CurrentLanguage = (Languages::eLangId)langId;
    MusicEnabled = musEnabled;
    BackgroundType = (BackgroundTypeEnum)backgroudType;
    PlayerName = playername;
#endif
    return NULL;
}

LPErrInApp GameSettings::SaveSettings() {
#if PLATFORM_EMS
    std::string val = MusicEnabled ? "true" : "false";
    saveGameSetting("MusicEnabled", val.c_str());
    val = std::to_string(CurrentLanguage);
    saveGameSetting("CurrentLanguage", val.c_str());
    val = std::to_string(DeckTypeVal.GetTypeIndex());
    saveGameSetting("DeckTypeVal", val.c_str());
    val = std::to_string(BackgroundType);
    saveGameSetting("BackgroundType", val.c_str());
    saveGameSetting("PlayerName", PlayerName.c_str());
#else
    LPErrInApp err = setSettingFileName();
    if (err != NULL) {
        return err;
    }

    TRACE("Save setting file %s\n", g_filepath);
    uint8_t deckId;
    uint8_t langId;
    uint8_t musEnabled;
    uint8_t backgroudType;

    deckId = (uint8_t)DeckTypeVal.GetTypeIndex();
    langId = (uint8_t)CurrentLanguage;
    musEnabled = (uint8_t)MusicEnabled;
    backgroudType = (uint8_t)BackgroundType;

    SDL_IOStream* dst = SDL_IOFromFile(g_filepath, "wb");
    if (dst == 0) {
        return ERR_UTIL::ErrorCreate("Unable to save setting file %s",
                                     g_filepath);
    }
    int numWritten = SDL_WriteIO(dst, &deckId, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }
    numWritten = SDL_WriteIO(dst, &langId, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }
    numWritten = SDL_WriteIO(dst, &musEnabled, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }
    numWritten = SDL_WriteIO(dst, &backgroudType, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }

    // save len + string
    uint8_t nameSize = PlayerName.size();
    numWritten = SDL_WriteIO(dst, &nameSize, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }

    numWritten = SDL_WriteIO(dst, PlayerName.c_str(), nameSize);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_WriteIO error %s\n", SDL_GetError());
    }
    SDL_CloseIO(dst);
#endif
    return NULL;
}

LPErrInApp GameSettings::LoadFonts() {
    TRACE_DEBUG("GameSettings::LoadFonts %s \n", g_lpszIniFontAriblkFname);
    _p_fontAriblk = TTF_OpenFont(g_lpszIniFontAriblkFname, _fontBigSize);
    if (_p_fontAriblk == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszIniFontAriblkFname, SDL_GetError());
    }
    TRACE_DEBUG("LoadFonts Ariblk OK \n");
    _p_fontVera = TTF_OpenFont(g_lpszIniFontVeraFname, _fontSmallSize);
    if (_p_fontVera == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszIniFontVeraFname, SDL_GetError());
    }
    _p_fontMedium = TTF_OpenFont(g_lpszIniFontVeraFname, _fontMediumSize);
    if (_p_fontMedium == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Unable to load medium font %s, error: %s\n",
            g_lpszIniFontVeraFname, SDL_GetError());
    }
    _p_fontSymb = TTF_OpenFont(g_lpszFontSymbFname, _fontSymSize);
    if (_p_fontSymb == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to load font %s, error: %s\n",
                                     g_lpszFontSymbFname, SDL_GetError());
    }
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

LPErrInApp GameSettings::CalcDisplaySize(int w, int h) {
    _screenRect.w = w;
    _screenRect.h = h;
#ifdef ANDROID
    int num_displays;
    SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);
    if (num_displays == 0) {
        return NULL;
    }
    SDL_DisplayID Id = *displays;
    SDL_Rect screenRect;
    if (!SDL_GetDisplayBounds(Id, &screenRect)) {
        return ERR_UTIL::ErrorCreate("CalcDisplaySize error: %s\n",
                                     SDL_GetError());
    }
    if (screenRect.w > _screenRect.w) {
        _screenRect.w = screenRect.w;
    }
    if (screenRect.h > _screenRect.h) {
        _screenRect.h = screenRect.h;
    }
    TRACE_DEBUG("Display bound for size is width %d, height %d", _screenRect.w,
                _screenRect.h);

    SDL_free(displays);
#endif
    return NULL;
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
