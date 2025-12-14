#include <SDL3/SDL.h>
// No more main() function, but instead 4 callbacks
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

#include "AppGfx.h"
#include "Config.h"
#include "ErrorInfo.h"

AppGfx* g_app = new AppGfx();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    SDL_AppResult res;
    TRACE("SDL_AppInit \n");
    g_app->ParseCmdLine(argc, argv, res);
    if (res == SDL_APP_SUCCESS) {
        return res;
    }
    LPErrInApp err = g_app->Init();
    if (err != NULL) {
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }
    TRACE("Initialization OK, ready for the main loop \n");
    return res;  // Note: SDL_APP_SUCCESS exit the app with
                 // success, here we need to continue
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    SDL_AppResult res;
    LPErrInApp err = g_app->MainLoopEvent(event, res);
    if (err != NULL) {
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }
    return res;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    LPErrInApp err = g_app->MainLoopIterate();
    if (err != NULL) {
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    TRACE("SDL_AppQuit \n");
    delete g_app;
}
