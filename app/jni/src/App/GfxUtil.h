#ifndef __GFX_UTIL__H
#define __GFX_UTIL__H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

#include "DeckType.h"
#include "ErrorInfo.h"

namespace GFX_UTIL {
Uint32 inline GetPixel(SDL_Surface* surface, int x, int y);
void inline SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

void rectangleRGBA(SDL_Surface* screen, int x, int y, int x2, int y2,
                   unsigned char r, unsigned char g, unsigned char b,
                   unsigned char a);
void boxRGBA(SDL_Surface* screen, int x1, int y1, int x2, int y2,
             unsigned char r, unsigned char g, unsigned char b,
             unsigned char a);

void ScreenShade(SDL_Surface* surface, SDL_Rect* rect, float opacity,
                 unsigned char r, unsigned char g, unsigned char b);
LPErrInApp DrawString(SDL_Surface* screen, const char* strText, int x, int y,
                      SDL_Color color, TTF_Font* customfont);
void DrawStaticSpriteEx(SDL_Surface* screen, int src_x, int src_y, int src_dx,
                        int src_dy, int dst_x, int dst_y, SDL_Surface* sprite);
void DrawStaticLine(SDL_Surface* screen, int x0, int y0, int x1, int y1,
                    SDL_Color color);
void DrawStaticBrokenLine(SDL_Surface* screen, int x0, int y0, int x1, int y1,
                          const SDL_Color* color1, const SDL_Color* color2,
                          int break_size);
void DrawRect(SDL_Surface* screen, int x, int y, int dx, int dy, SDL_Color c);
void FillRect(SDL_Surface* screen, int x0, int y0, int width, int height,
              Uint32 color);
LPErrInApp LoadCardPac(SDL_Surface** pp_Deck, DeckType& deckType, Uint16* pac_w,
                       Uint16* pac_h);
SDL_Surface* SDL_CreateRGBSurface(int width, int height, int depth,
                                  Uint32 Rmask, Uint32 Gmask, Uint32 Bmask,
                                  Uint32 Amask);
};  // namespace GFX_UTIL

class GFX_UTIL_COLOR {
   public:
    GFX_UTIL_COLOR() {}
    static SDL_Color White;
    static SDL_Color Gray;
    static SDL_Color Red;
    static SDL_Color Green;
    static SDL_Color Black;
    static SDL_Color Orange;
    static SDL_Color Blue;
};

namespace STR_UTIL {
std::string intToString(int iNumber);
};

#endif
