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

LPGameSettings GAMESET::GetSettings() {
    if (_p_GameSettings == NULL) {
        _p_GameSettings = new GameSettings();
    }
    return _p_GameSettings;
}

const char* GAMESET::GetExeSolitarioFolder() {
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

const char* GAMESET::GetNameWithAssets(const char *src_path){
#ifdef ANDROID
    char fpath[PATH_MAX];
    snprintf(fpath, sizeof(fpath), "%s/%s", SDL_AndroidGetInternalStoragePath(), src_path);
    std::string fwithAsset = fpath;
    return fwithAsset.c_str();
#else
    return src_path;
#endif
}

const char* GAMESET::GetHomeSolitarioFolder() {
    if (strlen(_settingsRootDir) > 0) {
        return _settingsRootDir;
    }
#ifdef ANDROID
    sprintf(_settingsRootDir, "%s/.solitario", SDL_AndroidGetInternalStoragePath());
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

LPErrInApp GAMESET::CreateHomeSolitarioFolderIfNotExists(bool& dirCreated) {
    dirCreated = false;
    struct stat st = {0};
    int io_res;
    GetHomeSolitarioFolder();
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
