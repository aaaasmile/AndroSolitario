#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

#include "AppGfx.h"
#include "Config.h"
#include "ErrorInfo.h"

AppGfx* g_app = new AppGfx();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    g_app->ParseCmdLine(argc, argv);
    LPErrInApp err = g_app->Init();
    if (err != NULL) {
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }
    TRACE("Initialization OK, ready for the main loop \n");
    return SDL_APP_CONTINUE; //Note: SDL_APP_SUCCESS exit the app with success, here we need to continue
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
    SDL_AppResult res;
    LPErrInApp err = g_app->MainLoopEvent(event, res);
    if (err != NULL) {
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }
    return res;
}

SDL_AppResult SDL_AppIterate(void *appstate){
    LPErrInApp err = g_app->MainLoopIterate();
    if (err != NULL) {
        TRACE("Fatal: %s\n", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    delete g_app;
}

// int main(int argc, char* argv[]) {
//     TRACE("*** [Main] - start *** \n");
//     AppGfx* app = new AppGfx();
//     app->ParseCmdLine(argc, argv);
//     LPErrInApp err = app->Init();
//     if (err != NULL) {
//         TRACE("Fatal: %s\n", err->ErrorText.c_str());
//         fprintf(stderr, "Init error: %s\n", err->ErrorText.c_str());
//         delete app;
//         return EXIT_FAILURE;
//     }
//     TRACE("Initialization OK, ready for the main loop \n");
//     err = app->MainLoop();
//     if (err != NULL) {
//         TRACE("Fatal: %s\n", err->ErrorText.c_str());
//         fprintf(stderr, "App error: %s\n", err->ErrorText.c_str());
//         delete app;
//         return EXIT_FAILURE;
//     }
//     delete app; // othewise (error=Try to release egl_surface with context
//     probably still active) TRACE("App terminated with success\n"); return
//     EXIT_SUCCESS;
// }
