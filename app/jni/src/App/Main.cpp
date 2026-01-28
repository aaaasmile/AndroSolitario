#include <SDL3/SDL.h>
// No more main() function, but instead 4 callbacks
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

#include "AppGfx.h"
#include "Config.h"
#include "ErrorInfo.h"

#if PLATFORM_EMS
#include <emscripten.h>
void saveErrorInLocalStorage(const char* section, const char* msg) {
    char js_cmd[1024];
    snprintf(js_cmd, sizeof(js_cmd), "localStorage.setItem('err_msg', '%s %s')",
             section, msg);
    emscripten_run_script(js_cmd);
}

#endif

AppGfx* g_app = new AppGfx();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    SDL_AppResult res;
    TRACE("[SDL_AppInit] - start \n");
    g_app->ParseCmdLine(argc, argv, res);
    if (res == SDL_APP_SUCCESS) {
        return res;
    }
    LPErrInApp err = g_app->Init();
    if (err != NULL) {
        TRACE("[SDL_AppInit] Fatal: %s\n", err->ErrorText.c_str());
#if PLATFORM_EMS
        saveErrorInLocalStorage("[SDL_AppInit] Fatal", err->ErrorText.c_str());
#endif
        return SDL_APP_FAILURE;
    }
    TRACE("[SDL_AppInit] - end \n");
    return res;  // Note: SDL_APP_SUCCESS exit the app with
                 // success, here we need to continue
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    SDL_AppResult res;
    LPErrInApp err = g_app->MainLoopEvent(event, res);
    if (err != NULL) {
        TRACE("[SDL_AppEvent] Fatal: %s\n", err->ErrorText.c_str());
#if PLATFORM_EMS
        saveErrorInLocalStorage("[SDL_AppEvent] Fatal", err->ErrorText.c_str());
#endif
        return SDL_APP_FAILURE;
    }
    return res;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    LPErrInApp err = g_app->MainLoopIterate();
    if (err != NULL) {
        TRACE("[SDL_AppIterate] Fatal: %s\n", err->ErrorText.c_str());
#if PLATFORM_EMS
        saveErrorInLocalStorage("[SDL_AppIterate] Fatal", err->ErrorText.c_str());
#endif
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    TRACE("SDL_AppQuit \n");
    delete g_app;
}
