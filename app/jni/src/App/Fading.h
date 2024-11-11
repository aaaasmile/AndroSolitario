#ifndef FADING_H
#define FADING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include <SDL3/SDL.h>

void Fade(SDL_Surface* p_surf_screen, SDL_Surface* p_surf_img,
          Uint32 ui_seconds, int i_fade_out, SDL_Renderer* psdlRenderer,
          SDL_Rect* prctTarget);

void InstantFade(SDL_Surface* p_surf_screen);

#ifdef __cplusplus
}
#endif
#endif
