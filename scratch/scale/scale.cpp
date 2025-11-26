#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "ErrorInfo.h"
#include "GfxUtil.h"

static SDL_Window* g_pWindow = NULL;
static SDL_Renderer* g_pRenderer = NULL;
static SDL_Texture* g_pTexture = NULL;

Uint16 g_pac_w, g_pac_h;
SDL_Surface* g_p_DeckSurf;
DeckType g_deckType;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if (!SDL_CreateWindowAndRenderer("Scale", 800, 600, SDL_WINDOW_RESIZABLE,
                                     &g_pWindow, &g_pRenderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    g_deckType.SetType(eDeckType::TAROCK_PIEMONT);
    LPErrInApp err = GFX_UTIL::LoadCardPac(&g_p_DeckSurf, g_deckType, &g_pac_w, &g_pac_h);
    if (err != NULL) {
        SDL_Log("error: %s", err->ErrorText.c_str());
        return SDL_APP_FAILURE;
    }

    g_pTexture = SDL_CreateTextureFromSurface(g_pRenderer, g_p_DeckSurf);
    if (g_pTexture == NULL) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
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

    SDL_FRect dst_rect;
    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(g_pRenderer);  /* start with a blank canvas. */

    /* Just draw the static texture a few times. You can think of it like a
       stamp, there isn't a limit to the number of times you can draw with it. */

    /* top left */
    dst_rect.x = 10.0;
    dst_rect.y = 10.0f;
    dst_rect.w = (float) g_pac_w;
    dst_rect.h = (float) g_pac_h;
    SDL_RenderTexture(g_pRenderer, g_pTexture, NULL, &dst_rect);

    SDL_RenderPresent(g_pRenderer);  /* put it all on the screen! */

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {}