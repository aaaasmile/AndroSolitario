#ifndef FADING_H___
#define FADING_H___

#include <SDL3/SDL.h>
#include <stdio.h>

#include "ErrorInfo.h"

class FadeAction {
   public:
    LPErrInApp Fade(SDL_Surface* pSurfScreen, SDL_Surface* pSurfImg,
                    Uint32 uiSeconds, bool fadeOut, SDL_Renderer* p_sdlRenderer,
                    SDL_Rect* p_rctTarget);

    void InstantFade(SDL_Surface* p_surf_screen);
    bool IsInProgress() { return _inProgress; }
    void Iterate();

   private:
    bool _inProgress;
    bool _fade_out;
    float _f_alpha;
    Uint64 _ui_old_time;
    Uint64 _ui_time_ms;
    SDL_Surface* _p_surf_screen;
    SDL_Surface* _p_surf_img;
    SDL_Surface* _p_surf_black;
    SDL_Texture* _p_ScreenTexture;
    SDL_Renderer* _p_sdlRenderer;
    SDL_Surface* _p_surf_screen_copy;
    SDL_Rect* _p_rctTarget;
};

#endif
