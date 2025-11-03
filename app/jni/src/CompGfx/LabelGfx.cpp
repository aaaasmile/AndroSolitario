#include "LabelGfx.h"

#include "GfxUtil.h"

LabelGfx::LabelGfx() {
    _StateGfx = INVISIBLE;
    _p_FontText = 0;
    _isEnabled = true;
    _p_Surf_Bar = 0;
}

LabelGfx::~LabelGfx() {
    if (_p_Surf_Bar) {
        SDL_DestroySurface(_p_Surf_Bar);
        _p_Surf_Bar = NULL;
    }
}

void LabelGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                          TTF_Font* pFont) {
    _rctCtrl = *pRect;

    // black bar surface
    // _p_Surf_Bar = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctCtrl.w, _rctCtrl.h,
    //                                    32, 0, 0, 0, 0);
    _p_Surf_Bar =
        SDL_CreateSurface(_rctCtrl.w, _rctCtrl.h, SDL_PIXELFORMAT_RGBA32);                            

    //SDL_FillRect(_p_Surf_Bar, NULL, SDL_MapRGBA(pScreen->format, 255, 0, 0, 0));
    SDL_FillSurfaceRect(
        _p_Surf_Bar, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                   SDL_GetSurfacePalette(pScreen), 255, 0, 0));

    SDL_SetSurfaceBlendMode(_p_Surf_Bar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_Surf_Bar, 127);
    _p_FontText = pFont;

    _color = GFX_UTIL_COLOR::White;
}

void LabelGfx::SetState(SateGfx eVal) {
    SateGfx eOldState = _StateGfx;
    _StateGfx = eVal;
    if (eOldState != _StateGfx && _StateGfx == VISIBLE) {
        _color = GFX_UTIL_COLOR::White;
    }
}

void LabelGfx::Draw(SDL_Surface* pScreen) {
    if (_StateGfx != INVISIBLE) {
        if (_isEnabled) {
            int tx, ty;
            //TTF_SizeText(_p_FontText, _CtrlText.c_str(), &tx, &ty); SDL2
            TTF_GetStringSize(_p_FontText, _CtrlText.c_str(), 0, &tx, &ty);
            int iXOffSet = (_rctCtrl.w - tx) / 2;
            if (iXOffSet < 0) {
                iXOffSet = 1;
            }
            int iYOffset = (_rctCtrl.h - ty) / 2;

            iYOffset = 0;
            iXOffSet = 0;
            GFX_UTIL::DrawString(pScreen, _CtrlText.c_str(),
                                 _rctCtrl.x + iXOffSet, _rctCtrl.y + iYOffset,
                                 _color, _p_FontText);
        }
    }
}