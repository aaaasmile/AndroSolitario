#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window* g_pWindow = NULL;
static SDL_Renderer* g_pRenderer = NULL;
static SDL_Texture* g_pTexture = NULL;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

    if (!SDL_CreateWindowAndRenderer("Scale", 800, 600,
                                     SDL_WINDOW_RESIZABLE, &g_pWindow,
                                     &g_pRenderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; 
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    int w = 0, h = 0;
    SDL_FRect dst;
    const float scale = 4.0f;

    SDL_GetRenderOutputSize(g_pRenderer, &w, &h);
    SDL_SetRenderScale(g_pRenderer, scale, scale);
    // SDL_GetTextureSize(texture, &dst.w, &dst.h);
    // dst.x = ((w / scale) - dst.w) / 2;
    // dst.y = ((h / scale) - dst.h) / 2;

    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(g_pRenderer);
    //SDL_RenderTexture(g_pRenderer, texture, NULL, &dst);
    SDL_RenderPresent(g_pRenderer);

    return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    
}