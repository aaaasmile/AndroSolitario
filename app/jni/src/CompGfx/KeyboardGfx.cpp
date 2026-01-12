#include "KeyboardGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

KeyboardGfx::KeyboardGfx() {
    _p_keyboardSurface = NULL;
    _p_fontText = NULL;
    _visibleState = VisbleState::VISIBLE;
    _fncbKeyboardEvent.self = NULL;
    _fncbKeyboardEvent.tc = NULL;
}

KeyboardGfx::~KeyboardGfx() {
    if (_p_keyboardSurface) {
        SDL_DestroySurface(_p_keyboardSurface);
        _p_keyboardSurface = NULL;
    }
}

void KeyboardGfx::Show(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont,
                       ClickKeyboardCb& fncbClickEvent) {
    _fncbKeyboardEvent = fncbClickEvent;
    _p_fontText = pFont;
    _rctCtrl = *pRect;

    _p_keyboardSurface =
        GFX_UTIL::SDL_CreateRGBSurface(_rctCtrl.w, _rctCtrl.h, 32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_keyboardSurface, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 0, 0));

    SDL_SetSurfaceBlendMode(_p_keyboardSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_keyboardSurface, 127);
}

void KeyboardGfx::DrawCtrl(SDL_Surface* pScreen) {
    if (_visibleState == VisbleState::INVISIBLE) {
        return;
    }
    // Draw background
    GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctCtrl.w, _rctCtrl.h,
                                 _rctCtrl.x, _rctCtrl.y, _p_keyboardSurface);

    // Draw border
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 1, _rctCtrl.y - 1,
                       _rctCtrl.x + _rctCtrl.w + 1, _rctCtrl.y + _rctCtrl.h + 1,
                       GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 2, _rctCtrl.y - 2,
                       _rctCtrl.x + _rctCtrl.w + 2, _rctCtrl.y + _rctCtrl.h + 2,
                       GFX_UTIL_COLOR::Black);
}

void KeyboardGfx::HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos) {
    if (_visibleState == VisbleState::INVISIBLE) {
        return;
    }
}