#include "Fading.h"

static SDL_Surface* SDL_CreateRGBSurface(int width, int height, int depth,
                                         Uint32 Rmask, Uint32 Gmask,
                                         Uint32 Bmask, Uint32 Amask) {
    return SDL_CreateSurface(
        width, height,
        SDL_GetPixelFormatForMasks(depth, Rmask, Gmask, Bmask, Amask));
}

// Fades the given surface in or out to the given screen within the given time
//  If the image surface is the screen surface (pointer are equal), a copy is
//  made first. We must do that because we are overwriting the Screen Surface.
void Fade(SDL_Surface* p_surf_screen, SDL_Surface* p_surf_img,
          Uint32 ui_seconds, int b_fade_out, SDL_Renderer* psdlRenderer,
          SDL_Rect* prctTarget) {
    // Becomes the black surface
    SDL_Surface* p_surf_black = NULL;
    SDL_Texture* pScreenTexture =
        SDL_CreateTextureFromSurface(psdlRenderer, p_surf_screen);

    // Used when the Screen Surface equals the Image Surface
    SDL_Surface* p_surf_screen_copy = NULL;
    // Used to calculate the steps to make a fade in the given time:
    Uint64 ui_old_time, ui_curr_time, ui_time_ms;
    float f_alpha;
    // Becomes flag to pass when creating a Surface
    // Uint32 ui_flags = SDL_SWSURFACE;
    // if ((p_surf_black = SDL_CreateRGBSurface(
    //          ui_flags, p_surf_screen->w, p_surf_screen->h,
    //          p_surf_screen->format->BitsPerPixel,
    //          p_surf_screen->format->Rmask, p_surf_screen->format->Gmask,
    //          p_surf_screen->format->Bmask, p_surf_screen->format->Amask)) ==
    //          NULL) {
    p_surf_black = SDL_CreateRGBSurface(
        p_surf_screen->w, p_surf_screen->h,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->bits_per_pixel,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Rmask,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Gmask,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Bmask,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Amask);

    // SDL_PIXELFORMAT_RGBA32);
    if (p_surf_black == NULL) {
        fprintf(stderr, "fade: could not create the black Surface. (%s)\n",
                SDL_GetError());
        return;
    }
    SDL_SetSurfaceBlendMode(p_surf_black, SDL_BLENDMODE_BLEND);
    // SDL_FillRect(p_surf_black, NULL,
    //              SDL_MapRGB(p_surf_screen->format, 0, 0, 0)); //SDL 2
    SDL_FillSurfaceRect(
        p_surf_black, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(p_surf_black->format), NULL, 0, 0,
                   0));

    if (p_surf_screen == p_surf_img) {
        // if ((p_surf_screen_copy = SDL_CreateRGBSurface(
        //          ui_flags, p_surf_screen->w, p_surf_screen->h,
        //          p_surf_screen->format->BitsPerPixel,
        //          p_surf_screen->format->Rmask, p_surf_screen->format->Gmask,
        //          p_surf_screen->format->Bmask, p_surf_screen->format->Amask))
        //          ==
        //     NULL) { //SDL 2
        p_surf_screen_copy = SDL_CreateRGBSurface(
            p_surf_screen->w, p_surf_screen->h,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->bits_per_pixel,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Rmask,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Gmask,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Bmask,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Amask);

        if (p_surf_screen_copy == NULL) {
            fprintf(
                stderr,
                "fade: could not create a copy of the Screen Surface. (%s)\n",
                SDL_GetError());
            SDL_DestroySurface(p_surf_black);
            return;
        }
        SDL_BlitSurface(p_surf_screen, NULL, p_surf_screen_copy, NULL);
        p_surf_img = p_surf_screen_copy;
    }
    // Ok, we are now ready for the fade. These are the steps (looped):
    // 1. Draw p_surf_img onto p_surf_screen, just an ordinary blit.
    // 2. Decrease or increase (depends on fading in or out) the alpha value,
    //   based on the elapsed time since the previous loop-iteration.
    // 3. Draw p_surf_black onto p_surf_screen in the current alpha value.
    ui_old_time = SDL_GetTicks();
    ui_curr_time = ui_old_time;
    // Convert the given time in seconds into miliseconds.
    ui_time_ms = ui_seconds * 1000;
    if (b_fade_out) {
        f_alpha = 0.0;
        // Loop until the alpha value exceeds 255 (this is the maximum alpha
        // value)
        while (f_alpha < 255.0) {
            SDL_BlitSurface(p_surf_img, NULL, p_surf_screen, prctTarget);
            SDL_SetSurfaceAlphaMod(p_surf_black, (Uint8)f_alpha);
            SDL_BlitSurface(p_surf_black, NULL, p_surf_screen, NULL);
            ui_old_time = ui_curr_time;
            ui_curr_time = SDL_GetTicks();
            SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
                              p_surf_screen->pitch);
            SDL_RenderTexture(psdlRenderer, pScreenTexture, NULL, NULL);
            SDL_RenderPresent(psdlRenderer);

            f_alpha += 255 * ((float)(ui_curr_time - ui_old_time) / ui_time_ms);
        }
    } else {
        f_alpha = 255.0;
        while (f_alpha > 0.0) {
            SDL_BlitSurface(p_surf_img, NULL, p_surf_screen, prctTarget);
            SDL_SetSurfaceAlphaMod(p_surf_black, (Uint8)f_alpha);
            SDL_BlitSurface(p_surf_black, NULL, p_surf_screen, NULL);
            ui_old_time = ui_curr_time;
            ui_curr_time = SDL_GetTicks();
            SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
                              p_surf_screen->pitch);
            SDL_RenderTexture(psdlRenderer, pScreenTexture, NULL, NULL);
            SDL_RenderPresent(psdlRenderer);

            f_alpha -= 255 * ((float)(ui_curr_time - ui_old_time) / ui_time_ms);
        }
    }
    SDL_DestroySurface(p_surf_black);
    if (p_surf_screen_copy != NULL) {
        SDL_DestroySurface(p_surf_screen_copy);
    }
    SDL_DestroyTexture(pScreenTexture);
}

void InstantFade(SDL_Surface* p_surf_screen) {
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = p_surf_screen->w;
    dest.h = p_surf_screen->h;
    // SDL_Surface* pBlack = SDL_CreateRGBSurface(
    //     SDL_SWSURFACE, p_surf_screen->w, p_surf_screen->h, 32, 0, 0, 0, 0);
    //     SDL2
    SDL_Surface* pBlack = SDL_CreateSurface(p_surf_screen->w, p_surf_screen->h,
                                            SDL_PIXELFORMAT_RGBA32);

    // SDL_FillRect(pBlack, &pBlack->clip_rect,
    //              SDL_MapRGBA(pBlack->format, 0, 0, 0, 0)); SDL2
    SDL_Rect clipRect;  // SDL 3
    SDL_GetSurfaceClipRect(pBlack, &clipRect);
    SDL_FillSurfaceRect(
        pBlack, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(pBlack->format), NULL, 0, 0, 0));

    SDL_BlitSurface(pBlack, NULL, p_surf_screen, &dest);

    SDL_DestroySurface(pBlack);
}
