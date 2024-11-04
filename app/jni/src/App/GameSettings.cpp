#include "GameSettings.h"

#include <SDL.h>
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

#include "WhereAmI.h"
#include "WinTypeGlobal.h"

static LPGameSettings _p_GameSettings = NULL;
static char _settingsRootDir[1024] = "";
static char _exeRootDir[1024] = "";
static char g_filepath[1024];

LPGameSettings GAMESET::GetSettings() {
    if (_p_GameSettings == NULL) {
        _p_GameSettings = new GameSettings();
    }
    return _p_GameSettings;
}

LPErrInApp GameSettings::LoadSettings() {
    if (SettingsDir == "" || GameName == "") {
        return ERR_UTIL::ErrorCreate(
            "Setting dir for user settings is not defined");
    }
    setSettingFileName();

    TRACE("Load setting file %s\n", g_filepath);
    SDL_RWops* src = SDL_RWFromFile(g_filepath, "rb");
    if (src == 0) {
        TRACE("No setting file found, no problem ignore it and use default\n");
        return NULL;
    }

    uint8_t deckId;
    uint8_t langId;
    uint8_t musEnabled;
    uint8_t backgroudType;

    if (SDL_RWread(src, &deckId, 1, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    if (SDL_RWread(src, &langId, 1, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    if (SDL_RWread(src, &musEnabled, 1, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    if (SDL_RWread(src, &backgroudType, 1, 1) == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWread on setting file error (file %s): %s\n", g_filepath,
            SDL_GetError());
    }
    SDL_RWclose(src);

    DeckTypeVal.SetTypeIndex(deckId);
    CurrentLanguage = (Languages::eLangId)langId;
    MusicEnabled = musEnabled;
    BackgroundType = (BackgroundTypeEnum)backgroudType;
    return NULL;
}

void GameSettings::setSettingFileName(void) {
    snprintf(g_filepath, sizeof(g_filepath), "%s/%s.bin", SettingsDir.c_str(),
             GameName.c_str());
}

LPErrInApp GameSettings::SaveSettings() {
    if (SettingsDir == "" || GameName == "") {
        return ERR_UTIL::ErrorCreate("User dir for setting is not defined");
    }
    setSettingFileName();

    TRACE("Save setting file %s\n", g_filepath);
    uint8_t deckId;
    uint8_t langId;
    uint8_t musEnabled;
    uint8_t backgroudType;

    deckId = (uint8_t)DeckTypeVal.GetTypeIndex();
    langId = (uint8_t)CurrentLanguage;
    musEnabled = (uint8_t)MusicEnabled;
    backgroudType = (uint8_t)BackgroundType;

    SDL_RWops* dst = SDL_RWFromFile(g_filepath, "wb");
    if (dst == 0) {
        return ERR_UTIL::ErrorCreate("Unable to save setting file %s",
                                     g_filepath);
    }
    int numWritten = SDL_RWwrite(dst, &deckId, 1, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_RWwrite single byte %s\n",
                                     SDL_GetError());
    }
    numWritten = SDL_RWwrite(dst, &langId, 1, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_RWwrite single byte %s\n",
                                     SDL_GetError());
    }
    numWritten = SDL_RWwrite(dst, &musEnabled, 1, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_RWwrite single byte %s\n",
                                     SDL_GetError());
    }
    numWritten = SDL_RWwrite(dst, &backgroudType, 1, 1);
    if (numWritten < 1) {
        return ERR_UTIL::ErrorCreate("SDL_RWwrite single byte %s\n",
                                     SDL_GetError());
    }
    SDL_RWclose(dst);
    return NULL;
}

void GameSettings::GetTouchPoint(SDL_TouchFingerEvent& tfinger,
                                 SDL_Point* pPoint) {
    // TRACE_DEBUG("tap on %f, %f - w/h %d/%d\n", tfinger.x, tfinger.y,
    //             _screenRect.w, _screenRect.h);
    pPoint->x = tfinger.x * _screenRect.w;
    pPoint->y = tfinger.y * _screenRect.h;
}

void GameSettings::CalcDisplaySize() {
    SDL_GetDisplayBounds(0, &_screenRect);
    TRACE_DEBUG("Display bound for size is width %d, height %d", _screenRect.w,
                _screenRect.h);
}

//  Namespace GAMESET

void GAMESET::GetNameWithAssets(const char* src_path, std::string& res) {
#ifdef ANDROID
    char fpath[PATH_MAX];
    snprintf(fpath, sizeof(fpath), "%s/%s", SDL_AndroidGetInternalStoragePath(),
             src_path);
    res = fpath;
#else
    res = src_path;
#endif
}

const char* GAMESET::GetExeAppFolder() {
    if (strlen(_exeRootDir) > 0) {
        return _exeRootDir;
    }

    int dirname_length;
    int length = wai_getExecutablePath(NULL, 0, NULL);
    char* path = (char*)malloc(length + 1);
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';

    TRACE("Exe path: %s\n", path);

    std::string exeFolder = path;
    std::replace(exeFolder.begin(), exeFolder.end(), '\\', '/');

    size_t iPos = exeFolder.find_last_of('/');
    exeFolder = exeFolder.substr(0, iPos);
    snprintf(_exeRootDir, sizeof(_exeRootDir), "%s", exeFolder.c_str());

    free(path);
    return _exeRootDir;
}

const char* GAMESET::GetHomeFolder() {
    if (strlen(_settingsRootDir) > 0) {
        return _settingsRootDir;
    }
#ifdef ANDROID
    sprintf(_settingsRootDir, "%s/.solitario",
            SDL_AndroidGetInternalStoragePath());
    return _settingsRootDir;
#endif
#ifdef WIN32
    sprintf(_settingsRootDir, "%s/%s/.solitario", getenv("HOMEDRIVE"),
            getenv("HOMEPATH"));
#else
    sprintf(_settingsRootDir, "%s/.solitario", getenv("HOME"));
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
