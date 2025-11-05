#include "ButtonGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

ButtonGfx::ButtonGfx() {
    _visibleState = INVISIBLE;
    _p_fontText = 0;
    _butID = 0;
    _enabled = true;
    _p_buttonSurface = 0;
    _mouseIsDown = false;
    _buttonType = TEXT_BUTTON;
    _mouseState = MouseState::OUTSIDE;
    _p_GameSettings = GAMESET::GetSettings();
}

ButtonGfx::~ButtonGfx() {
    if (_p_buttonSurface) {
        SDL_DestroySurface(_p_buttonSurface);
        _p_buttonSurface = NULL;
    }
}

void ButtonGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                           TTF_Font* pFont, int iButID,
                           ClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctButton = *pRect;

    // _p_buttonSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctButton.w,
    //                                         _rctButton.h, 32, 0, 0, 0, 0);
    _p_buttonSurface = GFX_UTIL::SDL_CreateRGBSurface(
        _rctButton.w, _rctButton.h, 32, 0, 0, 0, 0);

    // SDL_FillRect(_p_buttonSurface, NULL,
    //              SDL_MapRGBA(pScreen->format, 255, 0, 0, 0)); SDL 2
    SDL_FillSurfaceRect(_p_buttonSurface, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 0, 0));

    SDL_SetSurfaceBlendMode(_p_buttonSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_buttonSurface, 127);
    _p_fontText = pFont;
    _butID = iButID;
    _mouseIsDown = false;
    _buttonType = TEXT_BUTTON;
}

void ButtonGfx::InitializeAsSymbol(SDL_Rect* pRect, SDL_Surface* pScreen,
                                   TTF_Font* pFont, int iButID,
                                   ClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctButton = *pRect;

    _p_buttonSurface = GFX_UTIL::SDL_CreateRGBSurface(
        _rctButton.w, _rctButton.h, 32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_buttonSurface, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 0, 0));

    SDL_SetSurfaceBlendMode(_p_buttonSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_buttonSurface, 127);
    _p_fontText = pFont;
    _butID = iButID;
    _mouseIsDown = false;
    _buttonType = SYMBOL_BT;
}

bool ButtonGfx::MouseMove(SDL_Event& event) {
    if (_p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
        return false;
    }
    MouseState previous = _mouseState;
    if (_visibleState == VISIBLE && _enabled) {
        if (event.motion.x >= _rctButton.x &&
            event.motion.x <= _rctButton.x + _rctButton.w &&
            event.motion.y >= _rctButton.y &&
            event.motion.y <= _rctButton.y + _rctButton.h) {
            _mouseState = MouseState::INSIDE;
        } else {
            _mouseState = MouseState::OUTSIDE;
        }
    }

    return previous != _mouseState;
}

bool ButtonGfx::MouseDown(SDL_Event& event) {
    if (_p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
        return false;
    }
    _mouseIsDown = false;
    if (_visibleState == VISIBLE && _enabled) {
        if (event.motion.x >= _rctButton.x &&
            event.motion.x <= _rctButton.x + _rctButton.w &&
            event.motion.y >= _rctButton.y &&
            event.motion.y <= _rctButton.y + _rctButton.h) {
            _mouseIsDown = true;
        }
    }
    return _mouseIsDown;
}

static bool IsPointInsideButton(const SDL_Rect& rct, const SDL_Point& pt) {
    if (pt.x >= rct.x && pt.x <= rct.x + rct.w && pt.y >= rct.y &&
        pt.y <= rct.y + rct.h) {
        return true;
    }
    return false;
}

void ButtonGfx::FingerDown(SDL_Event& event) {
    SDL_Point pt;
    _p_GameSettings->GetTouchPoint(event.tfinger, &pt);
    if (IsPointInsideButton(_rctButton, pt)) {
        if ((_fncbClickEvent.tc) != NULL)
            (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _butID);
    }
}

bool ButtonGfx::MouseUp(SDL_Event& event) {
    if (_p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
        return false;
    }
    if (_visibleState == VISIBLE && _enabled) {
        SDL_Point pt = {(int)event.motion.x, (int)event.motion.y};
        if (IsPointInsideButton(_rctButton, pt)) {
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _butID);
            _mouseState = MouseState::INSIDE;
        } else {
            _mouseState = MouseState::OUTSIDE;
        }
    }
    _mouseIsDown = false;
    return _mouseState == MouseState::INSIDE;
}

void ButtonGfx::DrawButton(SDL_Surface* pScreen) {
    if (_visibleState == INVISIBLE) {
        return;
    }
    if (!_enabled) {
        return;
    }

    int mx, my;
    SDL_Color colorText;
    float fmx, fmy;
    SDL_GetMouseState(&fmx, &fmy);
    mx = (int)fmx;
    my = (int)fmy;
    SDL_Point pt = {mx, my};
    if (IsPointInsideButton(_rctButton, pt)) {
        colorText = GFX_UTIL_COLOR::Orange;
        _mouseState = MouseState::INSIDE;
    } else {
        colorText = GFX_UTIL_COLOR::White;
        _mouseState = MouseState::OUTSIDE;
    }

    GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctButton.w, _rctButton.h,
                                 _rctButton.x, _rctButton.y, _p_buttonSurface);
    int tx, ty;
    // TTF_SizeText(_p_fontText, _buttonText.c_str(), &tx, &ty); SDL 2
    if (_buttonType == TEXT_BUTTON){
        TTF_GetStringSize(_p_fontText, _buttonText.c_str(), 0, &tx, &ty);
    }else {
        TTF_MeasureString(_p_fontText, _buttonText.c_str(), 1, _rctButton.w, &tx, NULL);
        tx += 5;
    }
    

    int iXOffSet = (_rctButton.w - tx) / 2;
    if (iXOffSet < 0) {
        iXOffSet = 1;
    }
    int iYOffset = (_rctButton.h - ty) / 2;
    GFX_UTIL::DrawString(pScreen, _buttonText.c_str(), _rctButton.x + iXOffSet,
                         _rctButton.y + iYOffset, colorText, _p_fontText);

    // draw border
    GFX_UTIL::DrawRect(pScreen, _rctButton.x - 1, _rctButton.y - 1,
                       _rctButton.x + _rctButton.w + 1,
                       _rctButton.y + _rctButton.h + 1, GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(pScreen, _rctButton.x - 2, _rctButton.y - 2,
                       _rctButton.x + _rctButton.w + 2,
                       _rctButton.y + _rctButton.h + 2, GFX_UTIL_COLOR::Black);
    GFX_UTIL::DrawRect(pScreen, _rctButton.x, _rctButton.y,
                       _rctButton.x + _rctButton.w, _rctButton.y + _rctButton.h,
                       colorText);
}
