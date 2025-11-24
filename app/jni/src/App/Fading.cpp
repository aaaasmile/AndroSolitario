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
LPErrInApp FadeAction::Fade(SDL_Surface* p_surf_screen, SDL_Surface* p_surf_img,
                            Uint32 ui_seconds, bool fade_out,
                            SDL_Renderer* psdlRenderer, SDL_Rect* prctTarget) {
    if (_inProgress) {
        return ERR_UTIL::ErrorCreate(
            "Fade is already in progess, use iterate\n");
    }
    _inProgress = true;
    // Becomes the black surface
    SDL_Surface* p_surf_black = NULL;
    SDL_Texture* pScreenTexture =
        SDL_CreateTextureFromSurface(psdlRenderer, p_surf_screen);

    // Used when the Screen Surface equals the Image Surface
    SDL_Surface* p_surf_screen_copy = NULL;
    // Used to calculate the steps to make a fade in the given time:
    // Uint64 ui_old_time, ui_curr_time, ui_time_ms;
    // float f_alpha;
    // Becomes flag to pass when creating a Surface
    p_surf_black = SDL_CreateRGBSurface(
        p_surf_screen->w, p_surf_screen->h,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->bits_per_pixel,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Rmask,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Gmask,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Bmask,
        SDL_GetPixelFormatDetails(p_surf_screen->format)->Amask);

    if (p_surf_black == NULL) {
        return ERR_UTIL::ErrorCreate(
            "fade: could not create the black Surface. (%s)\n", SDL_GetError());
    }
    SDL_SetSurfaceBlendMode(p_surf_black, SDL_BLENDMODE_BLEND);
    SDL_FillSurfaceRect(
        p_surf_black, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(p_surf_black->format), NULL, 0, 0,
                   0));

    if (p_surf_screen == p_surf_img) {
        p_surf_screen_copy = SDL_CreateRGBSurface(
            p_surf_screen->w, p_surf_screen->h,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->bits_per_pixel,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Rmask,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Gmask,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Bmask,
            SDL_GetPixelFormatDetails(p_surf_screen->format)->Amask);

        if (p_surf_screen_copy == NULL) {
            SDL_DestroySurface(p_surf_black);
            return ERR_UTIL::ErrorCreate(
                "fade: could not create a copy of the Screen Surface. (%s)\n",
                SDL_GetError());
        }
        SDL_BlitSurface(p_surf_screen, NULL, p_surf_screen_copy, NULL);
        p_surf_img = p_surf_screen_copy;
    }
    // Ok, we are now ready for the fade. These are the steps (looped):
    // 1. Draw p_surf_img onto p_surf_screen, just an ordinary blit.
    // 2. Decrease or increase (depends on fading in or out) the alpha value,
    //   based on the elapsed time since the previous loop-iteration.
    // 3. Draw p_surf_black onto p_surf_screen in the current alpha value.
    _ui_old_time = SDL_GetTicks();
    // ui_curr_time = ui_old_time;
    //  Convert the given time in seconds into miliseconds.
    _ui_time_ms = ui_seconds * 1000;
    if (fade_out) {
        _f_alpha = 0.0;
        // Loop until the alpha value exceeds 255 (this is the maximum alpha
        // value)
        // while (f_alpha < 255.0) {
        //     SDL_BlitSurface(p_surf_img, NULL, p_surf_screen, prctTarget);
        //     SDL_SetSurfaceAlphaMod(p_surf_black, (Uint8)f_alpha);
        //     SDL_BlitSurface(p_surf_black, NULL, p_surf_screen, NULL);
        //     ui_old_time = ui_curr_time;
        //     ui_curr_time = SDL_GetTicks();
        //     SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
        //                       p_surf_screen->pitch);
        //     SDL_RenderTexture(psdlRenderer, pScreenTexture, NULL, NULL);
        //     SDL_RenderPresent(psdlRenderer);

        //     f_alpha += 255 * ((float)(ui_curr_time - ui_old_time) /
        //     ui_time_ms);
        // }
    } else {
        _f_alpha = 255.0;
        // while (f_alpha > 0.0) {
        //     SDL_BlitSurface(p_surf_img, NULL, p_surf_screen, prctTarget);
        //     SDL_SetSurfaceAlphaMod(p_surf_black, (Uint8)f_alpha);
        //     SDL_BlitSurface(p_surf_black, NULL, p_surf_screen, NULL);
        //     ui_old_time = ui_curr_time;
        //     ui_curr_time = SDL_GetTicks();
        //     SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
        //                       p_surf_screen->pitch);
        //     SDL_RenderTexture(psdlRenderer, pScreenTexture, NULL, NULL);
        //     SDL_RenderPresent(psdlRenderer);

        //     f_alpha -= 255 * ((float)(ui_curr_time - ui_old_time) /
        //     ui_time_ms);
        // }
    }
    // SDL_DestroySurface(p_surf_black);
    // if (p_surf_screen_copy != NULL) {
    //     SDL_DestroySurface(p_surf_screen_copy);
    // }
    // SDL_DestroyTexture(pScreenTexture);

    return NULL;
}

void FadeAction::Iterate() {
    Uint64 ui_curr_time;
    if (_fade_out) {
        if (_f_alpha < 255.0) {
            SDL_BlitSurface(p_surf_img, NULL, p_surf_screen, prctTarget);
            SDL_SetSurfaceAlphaMod(p_surf_black, (Uint8)_f_alpha);
            SDL_BlitSurface(p_surf_black, NULL, p_surf_screen, NULL);
            SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
                              p_surf_screen->pitch);
            SDL_RenderTexture(psdlRenderer, pScreenTexture, NULL, NULL);
            SDL_RenderPresent(psdlRenderer);
            ui_curr_time = SDL_GetTicks();

            _f_alpha +=
                255 * ((float)(ui_curr_time - _ui_old_time) / _ui_time_ms);
            _ui_old_time = ui_curr_time;
        } else {
            _inProgress = false;
        }
    } else {
        if (_f_alpha > 0.0) {
            SDL_BlitSurface(p_surf_img, NULL, p_surf_screen, prctTarget);
            SDL_SetSurfaceAlphaMod(p_surf_black, (Uint8)_f_alpha);
            SDL_BlitSurface(p_surf_black, NULL, p_surf_screen, NULL);
            ui_curr_time = SDL_GetTicks();
            SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
                              p_surf_screen->pitch);
            SDL_RenderTexture(psdlRenderer, pScreenTexture, NULL, NULL);
            SDL_RenderPresent(psdlRenderer);

            ui_curr_time = SDL_GetTicks();
            _f_alpha -=
                255 * ((float)(ui_curr_time - _ui_old_time) / _ui_time_ms);
            _ui_old_time = ui_curr_time;
        } else {
            _inProgress = false;
        }
    }
    if (!_inProgress) {
        if (p_surf_black != NULL) {
            SDL_DestroySurface(p_surf_black);
            p_surf_black = NULL;
        }
        if (p_surf_screen_copy != NULL) {
            SDL_DestroySurface(p_surf_screen_copy);
            p_surf_screen_copy = NULL;
        }
        if(pScreenTexture != NULL){
            SDL_DestroyTexture(pScreenTexture);
            pScreenTexture = NULL;
        }
    }
}

void FadeAction::InstantFade(SDL_Surface* p_surf_screen) {
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = p_surf_screen->w;
    dest.h = p_surf_screen->h;
    SDL_Surface* pBlack = SDL_CreateSurface(p_surf_screen->w, p_surf_screen->h,
                                            SDL_PIXELFORMAT_RGBA32);

    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(pBlack, &clipRect);
    SDL_FillSurfaceRect(
        pBlack, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(pBlack->format), NULL, 0, 0, 0));

    SDL_BlitSurface(pBlack, NULL, p_surf_screen, &dest);

    SDL_DestroySurface(pBlack);
}
