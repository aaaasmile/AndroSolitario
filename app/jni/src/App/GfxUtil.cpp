#include "GfxUtil.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cmath>

#include "WinTypeGlobal.h"

const char* g_lpszDeckDir = DATA_PREFIX "mazzi/";

SDL_Color GFX_UTIL_COLOR::White = {255, 255, 255, 0};
SDL_Color GFX_UTIL_COLOR::Gray = {128, 128, 128, 0};
SDL_Color GFX_UTIL_COLOR::Red = {255, 0, 0, 0};
SDL_Color GFX_UTIL_COLOR::Green = {0, 255, 0, 0};
SDL_Color GFX_UTIL_COLOR::Black = {0, 0, 0, 0};
SDL_Color GFX_UTIL_COLOR::Orange = {255, 200, 0, 0};
SDL_Color GFX_UTIL_COLOR::Blue = {0, 0, 255, 0};
SDL_Color GFX_UTIL_COLOR::CustomLbl = {245, 200, 10, 0};//rgba(255, 204, 19, 1)

void GFX_UTIL::ScreenShade(SDL_Surface* surface, SDL_Rect* rect, float opacity,
                           unsigned char r1, unsigned char g1,
                           unsigned char b1) {
    Uint32 pixel;
    Uint8 r, g, b;
    bool bunlock = false;
    if (surface == NULL)
        return;
    if (SDL_LockSurface(surface) == 0)
        bunlock = true;
    // SET TRANSPARENCY FOR THE BOARD AREA
    for (int y = rect->y; y < rect->y + rect->h; y++)
        for (int x = rect->x; x < rect->x + rect->w; x++) {
            pixel = GetPixel(surface, x, y);
            SDL_GetRGB(pixel, SDL_GetPixelFormatDetails(surface->format), NULL,
                       &r, &g, &b);
            r = (Uint8)((float)r * opacity);
            g = (Uint8)((float)g * opacity);
            b = (Uint8)((float)b * opacity);
            Uint32 color = SDL_MapRGBA(
                SDL_GetPixelFormatDetails(surface->format), NULL, r, g, b, 255);
            SetPixel(surface, x, y, color);
        }
    if (bunlock == true)
        SDL_UnlockSurface(surface);
    rectangleRGBA(surface, rect->x, rect->y, rect->x + rect->w,
                  rect->y + rect->h, 0, 0, 0, 255);
    rectangleRGBA(surface, rect->x + 1, rect->y + 1, rect->x + rect->w - 2,
                  rect->y + rect->h - 2, r1, g1, b1, 255);
}

void GFX_UTIL::rectangleRGBA(SDL_Surface* screen, int x1, int y1, int x2,
                             int y2, unsigned char r, unsigned char g,
                             unsigned char b, unsigned char a) {
    int x, y;
    Uint32 color;
    bool bneedunlock = false;
    if (screen == NULL)
        return;
    if (SDL_LockSurface(screen) == 0)
        bneedunlock = true;

    color = SDL_MapRGBA(SDL_GetPixelFormatDetails(screen->format), NULL, r, g,
                        b, a);

    for (x = x1; x < x2; x++) {
        SetPixel(screen, x, y1, color);
        SetPixel(screen, x, y2 - 1, color);
    }

    for (y = y1; y < y2; y++) {
        SetPixel(screen, x1, y, color);
        SetPixel(screen, x2 - 1, y, color);
    }
    if (bneedunlock == true)
        SDL_UnlockSurface(screen);
}

void GFX_UTIL::boxRGBA(SDL_Surface* screen, int x1, int y1, int x2, int y2,
                       unsigned char r, unsigned char g, unsigned char b,
                       unsigned char a) {
    int x, y;
    Uint32 color;
    bool bneedunlock = false;
    if (screen == NULL)
        return;

    if (SDL_LockSurface(screen) == 0)
        bneedunlock = true;

    color = SDL_MapRGBA(SDL_GetPixelFormatDetails(screen->format), NULL, r, g,
                        b, a);

    for (y = y1; y < y2; y++) {
        for (x = x1; x < x2; x++) {
            SetPixel(screen, x, y, color);
        }
    }
    if (bneedunlock == true)
        SDL_UnlockSurface(screen);
}

Uint32 inline GFX_UTIL::GetPixel(SDL_Surface* surface, int x, int y) {
    int bpp = SDL_GetPixelFormatDetails(surface->format)->bytes_per_pixel;
    // Here p is the address to the pixel we want to retrieve
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;

        case 2:
            return *(Uint16*)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32*)p;

        default:
            return 0;  // shouldn't happen, but avoids warnings
    }
}
void inline GFX_UTIL::SetPixel(SDL_Surface* surface, int x, int y,
                               Uint32 pixel) {
    int bpp = SDL_GetPixelFormatDetails(surface->format)->bytes_per_pixel;
    // Here p is the address to the pixel we want to set
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16*)p = pixel;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32*)p = pixel;
            break;
    }
}

LPErrInApp GFX_UTIL::DrawString(SDL_Surface* screen, const char* strText, int x,
                                int y, SDL_Color color, TTF_Font* customfont) {
    int width, height;
    TTF_GetStringSize(customfont, strText, 0, &width, &height);
    SDL_Surface* surFont;
    surFont = TTF_RenderText_Blended(customfont, strText, 0, color);
    if (surFont == NULL) {
        return ERR_UTIL::ErrorCreate("Error TTF_RenderUTF8_Blended: %s\n",
                                     SDL_GetError());
    }
    GFX_UTIL::DrawStaticSpriteEx(screen, 0, 0, width, height, x, y, surFont);
    SDL_DestroySurface(surFont);
    return NULL;
}

void GFX_UTIL::DrawStaticSpriteEx(SDL_Surface* screen, int src_x, int src_y,
                                  int src_dx, int src_dy, int dst_x, int dst_y,
                                  SDL_Surface* sprite) {
    SDL_Rect src_rec = {src_x, src_y, src_dx, src_dy};
    SDL_Rect dst_rec = {dst_x, dst_y, 0, 0};
    SDL_BlitSurface(sprite, &src_rec, screen, &dst_rec);
}

void GFX_UTIL::DrawStaticLine(SDL_Surface* screen, int x0, int y0, int x1,
                              int y1, SDL_Color color) {
    int d =
        (int)sqrtf(pow((float)(x1 - x0), 2.0f) + pow((float)(y1 - y0), 2.0f));
    static int x = 0, y = 0;
    static int w = screen->w;
    static int h = screen->h;
    for (int t = 0; t < d; t++) {
        x = x0 + (x1 - x0) * t / d;
        y = y0 + (y1 - y0) * t / d;
        if ((x >= 0) && (y >= 0) && (x < w) && (y < h)) {
            GFX_UTIL::SetPixel(
                screen, x, y,
                SDL_MapRGB(SDL_GetPixelFormatDetails(screen->format), NULL,
                           color.r, color.g, color.b));
        }
    }
}

void GFX_UTIL::DrawRect(SDL_Surface* screen, int xi, int yi, int xf, int yf,
                        SDL_Color color) {
    DrawStaticLine(screen, xi, yi, xf, yi, color);
    DrawStaticLine(screen, xi, yi, xi, yf, color);
    DrawStaticLine(screen, xf, yi, xf, yf, color);
    DrawStaticLine(screen, xi, yf, xf, yf, color);
}

void GFX_UTIL::FillRect(SDL_Surface* screen, int x0, int y0, int width,
                        int height, Uint32 color) {
    SDL_Rect rect = {x0, y0, width, height};
    SDL_FillSurfaceRect(screen, &rect, color);
}

LPErrInApp GFX_UTIL::LoadCardPac(SDL_Surface** pp_Deck, DeckType& deckType,
                                 Uint16* pac_w, Uint16* pac_h) {
    TRACE("Load card Pac %s\n", deckType.GetDeckName().c_str());
    Uint32 timetag;
    char description[100];
    Uint8 num_anims;
    Uint16 w, h;
    Uint16 frames;
    Uint16 ignore;

    std::string strFileName = g_lpszDeckDir;
    strFileName += deckType.GetResFileName();

    SDL_IOStream* src = SDL_IOFromFile(strFileName.c_str(), "rb");
    if (src == 0) {
        return ERR_UTIL::ErrorCreate(
            "SDL_RWFromFile on pac file error (file %s): %s\n",
            strFileName.c_str(), SDL_GetError());
    }
    SDL_ReadIO(src, description, 100);
    SDL_ReadU32LE(src, &timetag);
    TRACE_DEBUG("Timetag is %d\n", timetag);
    SDL_ReadIO(src, &num_anims, 1);
    // width of the picture (pac of 4 cards)
    SDL_ReadU16LE(src, &w);
    // height of the picture (pac of 10 rows of cards)
    SDL_ReadU16LE(src, &h);
    SDL_ReadU16LE(src, &frames);

    for (int i = 0; i < frames; i++) {
        SDL_ReadU16LE(src, &ignore);
    }

    *pp_Deck = IMG_LoadTyped_IO(src, false, "PNG");
    if (!*pp_Deck) {
        SDL_CloseIO(src);
        return ERR_UTIL::ErrorCreate(
            "IMG_LoadPNG_RW on pac file error (file %s): %s\n",
            strFileName.c_str(), SDL_GetError());
    }
    Uint8 red_trasp = 0, green_trasp = 128, blue_trasp = 0;
    if (deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
        red_trasp = 248;
        green_trasp = 0;
        blue_trasp = 241;
    }
    SDL_SetSurfaceColorKey(
        *pp_Deck, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails((*pp_Deck)->format),
                   SDL_GetSurfacePalette(*pp_Deck), red_trasp, green_trasp,
                   blue_trasp));
    *pac_h = h;
    *pac_w = w;

    SDL_CloseIO(src);
    return NULL;
}

void GFX_UTIL::DrawStaticBrokenLine(SDL_Surface* screen, int x0, int y0, int x1,
                                    int y1, const SDL_Color* color1,
                                    const SDL_Color* color2, int break_size) {}

std::string STR_UTIL::intToString(int iNumber) {
    std::string temp;
    char buffer[15];
    sprintf(buffer, "%d", iNumber);
    temp = buffer;
    return temp;
}

SDL_Surface* GFX_UTIL::SDL_CreateRGBSurface(int width, int height, int depth,
                                            Uint32 Rmask, Uint32 Gmask,
                                            Uint32 Bmask, Uint32 Amask) {
    return SDL_CreateSurface(
        width, height,
        SDL_GetPixelFormatForMasks(depth, Rmask, Gmask, Bmask, Amask));
}
