#ifndef FADING_H___
#define FADING_H___

#include <SDL3/SDL.h>
#include <stdio.h>

#include "ErrorInfo.h"

class FadeAction {
   public:
    LPErrInApp Fade(SDL_Surface* p_surf_screen, SDL_Surface* p_surf_img,
                    Uint32 ui_seconds, bool fade_out,
                    SDL_Renderer* psdlRenderer, SDL_Rect* prctTarget);

    void InstantFade(SDL_Surface* p_surf_screen);
    bool IsInProgress() { return _inProgress; }
    void Iterate();

   private:
    bool _inProgress;
    bool _fade_out;
    float _f_alpha;
    Uint64 _ui_old_time;
    Uint64 _ui_time_ms;
};

#endif
