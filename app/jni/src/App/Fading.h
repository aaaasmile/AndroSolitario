#ifndef FADING_H___
#define FADING_H__

#include <SDL3/SDL.h>
#include <stdio.h>

class FadeAction {
   public:
    void Fade(SDL_Surface* p_surf_screen, SDL_Surface* p_surf_img,
              Uint32 ui_seconds, int i_fade_out, SDL_Renderer* psdlRenderer,
              SDL_Rect* prctTarget);

    void InstantFade(SDL_Surface* p_surf_screen);
};

#endif
