#include "Fading.h"

#include <inttypes.h>

#include "WinTypeGlobal.h"

static SDL_Surface* SDL_CreateRGBSurface(int width, int height, int depth,
                                         Uint32 Rmask, Uint32 Gmask,
                                         Uint32 Bmask, Uint32 Amask) {
    return SDL_CreateSurface(
        width, height,
        SDL_GetPixelFormatForMasks(depth, Rmask, Gmask, Bmask, Amask));
}

FadeAction::~FadeAction() { cleanUp(); }

void FadeAction::cleanUp() {
    if (_p_surf_black != NULL) {
        SDL_DestroySurface(_p_surf_black);
        _p_surf_black = NULL;
    }
    if (_p_surf_screen_copy != NULL) {
        SDL_DestroySurface(_p_surf_screen_copy);
        _p_surf_screen_copy = NULL;
    }
}

// Fades the given surface in or out to the given screen within the given time
//  If the image surface is the screen surface (pointer are equal), a copy is
//  made first. We must do that because we are overwriting the Screen Surface.
LPErrInApp FadeAction::Fade(SDL_Surface* pSurfScreen, SDL_Surface* pSurfImg,
                            Uint32 uiSeconds, bool fadeOut,
                            //SDL_Renderer* p_sdlRenderer,
                            UpdateScreenCb& fnUpdateScreen,
                            SDL_Rect* p_rctTarget) {
    if (_inProgress) {
        return ERR_UTIL::ErrorCreate(
            "Fade is already in progess, use iterate\n");
    }
    _fnUpdateScreen = fnUpdateScreen;
    _inProgress = true;
    _fade_out = fadeOut;
    _p_surf_screen = pSurfScreen;
    _p_surf_img = pSurfImg;
    _p_rctTarget = p_rctTarget;
    _p_surf_screen_copy = NULL;

 
    // Used when the Screen Surface equals the Image Surface
    // Used to calculate the steps to make a fade in the given time:
    // Uint64 ui_old_time, ui_curr_time, ui_time_ms;
    // float f_alpha;
    // Becomes flag to pass when creating a Surface
    _p_surf_black = SDL_CreateRGBSurface(
        _p_surf_screen->w, _p_surf_screen->h,
        SDL_GetPixelFormatDetails(_p_surf_screen->format)->bits_per_pixel,
        SDL_GetPixelFormatDetails(_p_surf_screen->format)->Rmask,
        SDL_GetPixelFormatDetails(_p_surf_screen->format)->Gmask,
        SDL_GetPixelFormatDetails(_p_surf_screen->format)->Bmask,
        SDL_GetPixelFormatDetails(_p_surf_screen->format)->Amask);

    if (_p_surf_black == NULL) {
        return ERR_UTIL::ErrorCreate(
            "fade: could not create the black Surface. (%s)\n", SDL_GetError());
    }
    SDL_SetSurfaceBlendMode(_p_surf_black, SDL_BLENDMODE_BLEND);
    SDL_FillSurfaceRect(
        _p_surf_black, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_surf_black->format), NULL, 0, 0,
                   0));

    if (_p_surf_screen == _p_surf_img) {
        _p_surf_screen_copy = SDL_CreateRGBSurface(
            _p_surf_screen->w, _p_surf_screen->h,
            SDL_GetPixelFormatDetails(_p_surf_screen->format)->bits_per_pixel,
            SDL_GetPixelFormatDetails(_p_surf_screen->format)->Rmask,
            SDL_GetPixelFormatDetails(_p_surf_screen->format)->Gmask,
            SDL_GetPixelFormatDetails(_p_surf_screen->format)->Bmask,
            SDL_GetPixelFormatDetails(_p_surf_screen->format)->Amask);

        if (_p_surf_screen_copy == NULL) {
            SDL_DestroySurface(_p_surf_black);
            return ERR_UTIL::ErrorCreate(
                "fade: could not create a copy of the Screen Surface. (%s)\n",
                SDL_GetError());
        }
        SDL_BlitSurface(_p_surf_screen, NULL, _p_surf_screen_copy, NULL);
        _p_surf_img = _p_surf_screen_copy;
    }
    // Ok, we are now ready for the fade. These are the steps (looped):
    // 1. Draw p_surf_img onto p_surf_screen, just an ordinary blit.
    // 2. Decrease or increase (depends on fading in or out) the alpha value,
    //   based on the elapsed time since the previous loop-iteration.
    // 3. Draw p_surf_black onto p_surf_screen in the current alpha value.
    _ui_old_time = SDL_GetTicks();
    //  Convert the given time in seconds into miliseconds.
    _ui_time_ms = uiSeconds * 1000;
    TRACE_DEBUG("[Fade] Ticks initial %" PRIu64 ", end of fading %" PRIu64 ", fading out %d\n",
                _ui_old_time, _ui_time_ms, fadeOut);
    if (fadeOut) {
        _f_alpha = 0.0;
    } else {
        _f_alpha = 255.0;
    }

    return NULL;
}

void FadeAction::Iterate() {
    Uint64 ui_curr_time;
    if (_fade_out) {
        if (_f_alpha < 255.0) {
            SDL_BlitSurface(_p_surf_img, NULL, _p_surf_screen, _p_rctTarget);
            SDL_SetSurfaceAlphaMod(_p_surf_black, (Uint8)_f_alpha);
            SDL_BlitSurface(_p_surf_black, NULL, _p_surf_screen, NULL);
            (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_surf_screen);
            ui_curr_time = SDL_GetTicks();

            _f_alpha +=
                255 * ((float)(ui_curr_time - _ui_old_time) / _ui_time_ms);
            _ui_old_time = ui_curr_time;
        } else {
            _inProgress = false;
        }
    } else {
        if (_f_alpha > 0.0) {
            SDL_BlitSurface(_p_surf_img, NULL, _p_surf_screen, _p_rctTarget);
            SDL_SetSurfaceAlphaMod(_p_surf_black, (Uint8)_f_alpha);
            SDL_BlitSurface(_p_surf_black, NULL, _p_surf_screen, NULL);
            ui_curr_time = SDL_GetTicks();
            (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_surf_screen);

            ui_curr_time = SDL_GetTicks();
            _f_alpha -=
                255 * ((float)(ui_curr_time - _ui_old_time) / _ui_time_ms);
            _ui_old_time = ui_curr_time;
        } else {
            _inProgress = false;
        }
    }
    if (!_inProgress) {
        TRACE_DEBUG("[Fade] process terminated\n");
        cleanUp();
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
