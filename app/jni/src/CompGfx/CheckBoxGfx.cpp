#include "CheckBoxGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

CheckBoxGfx::CheckBoxGfx() {
    _visibleState = INVISIBLE;
    _p_FontText = 0;
    _enabled = true;
    _buttonID = -1;
    _p_GameSettings = GameSettings::GetSettings();
}

CheckBoxGfx::~CheckBoxGfx() {}

void CheckBoxGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                             TTF_Font* pFont, int iButID,
                             CheckboxClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctCtrl = *pRect;

    _p_FontText = pFont;

    _color = GFX_UTIL_COLOR::White;
    _colorBorder = _color;

    _clicked = false;

    _buttonID = iButID;
}

void CheckBoxGfx::SetVisibleState(VisbleState eVal) {
    VisbleState eOldState = _visibleState;
    _visibleState = eVal;
    if (eOldState != _visibleState && _visibleState == VISIBLE) {
        _color = GFX_UTIL_COLOR::White;
    }
}

static bool IsPointInsideCtrl(const SDL_Rect& rct, const SDL_Point& pt) {
    if (pt.x >= rct.x && pt.x <= rct.x + rct.w && pt.y >= rct.y &&
        pt.y <= rct.y + rct.h) {
        return true;
    }
    return false;
}

void CheckBoxGfx::FingerDown(SDL_Event* pEvent) {
    SDL_Point pt;
    _p_GameSettings->GetTouchPoint(pEvent->tfinger, &pt);
    if (IsPointInsideCtrl(_rctCtrl, pt)) {
        if (_clicked) {
            _clicked = false;
        } else {
            _clicked = true;
        }
        if ((_fncbClickEvent.tc) != NULL)
            (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _clicked);
    }
}

void CheckBoxGfx::MouseMove(SDL_Event* pEvent, SDL_Surface* pScreen,
                            SDL_Surface* pScene_background) {}

void CheckBoxGfx::MouseUp(SDL_Event* pEvent) {
    if (_p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
        return;
    }
    if (_visibleState == VISIBLE && _enabled) {
        if (pEvent->motion.x >= _rctCtrl.x &&
            pEvent->motion.x <= _rctCtrl.x + _rctCtrl.w &&
            pEvent->motion.y >= _rctCtrl.y &&
            pEvent->motion.y <= _rctCtrl.y + _rctCtrl.h) {
            if (_clicked) {
                _clicked = false;
            } else {
                _clicked = true;
            }
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _clicked);
        }
    }
}

void CheckBoxGfx::DrawButton(SDL_Surface* pScreen) {
    if (_visibleState == INVISIBLE) {
        return;
    }
    if (!_enabled) {
        return;
    }
    int xOffset = 0;
    int yOffset = 0;
    int ckbtW = 16;
    int ckbtH = 16;
    int intraOffsetX = 10;
    //LPGameSettings pGameSettings = GameSettings::GetSettings();
    // if (pGameSettings->NeedScreenMagnify()) {
    //     ckbtW = 40;
    //     ckbtH = 40;
    //     intraOffsetX = 40;
    // }
    SDL_Rect rctCheck;
    if (_clicked) {
        rctCheck.x = _rctCtrl.x;
        rctCheck.y = _rctCtrl.y;
        rctCheck.w = ckbtW;
        rctCheck.h = ckbtH;

        GFX_UTIL::DrawRect(pScreen, rctCheck.x - 1, rctCheck.y - 1,
                           rctCheck.x + rctCheck.w + 1,
                           rctCheck.y + rctCheck.h + 1, GFX_UTIL_COLOR::Gray);
        GFX_UTIL::DrawRect(pScreen, rctCheck.x - 2, rctCheck.y - 2,
                           rctCheck.x + rctCheck.w + 2,
                           rctCheck.y + rctCheck.h + 2, GFX_UTIL_COLOR::Black);
        GFX_UTIL::DrawStaticLine(
            pScreen, rctCheck.x, rctCheck.y, rctCheck.x + rctCheck.w,
            rctCheck.y + rctCheck.h, GFX_UTIL_COLOR::Orange);
        GFX_UTIL::DrawStaticLine(pScreen, rctCheck.x + rctCheck.w, rctCheck.y,
                                 rctCheck.x, rctCheck.y + rctCheck.h,
                                 GFX_UTIL_COLOR::Orange);

    } else {
        rctCheck.x = _rctCtrl.x;
        rctCheck.y = _rctCtrl.y;
        rctCheck.w = ckbtW;
        rctCheck.h = ckbtH;

        GFX_UTIL::DrawRect(pScreen, rctCheck.x - 1, rctCheck.y - 1,
                           rctCheck.x + rctCheck.w + 1,
                           rctCheck.y + rctCheck.h + 1, GFX_UTIL_COLOR::Gray);
        GFX_UTIL::DrawRect(pScreen, rctCheck.x - 2, rctCheck.y - 2,
                           rctCheck.x + rctCheck.w + 2,
                           rctCheck.y + rctCheck.h + 2, GFX_UTIL_COLOR::Black);
    }
    xOffset = rctCheck.w + intraOffsetX;
    yOffset = rctCheck.h;

    int tx, ty;
    TTF_GetStringSize(_p_FontText, _buttonText.c_str(), 0, &tx, &ty);

    if (xOffset < 0) {
        xOffset = 1;
    }
    GFX_UTIL::DrawString(pScreen, _buttonText.c_str(), _rctCtrl.x + xOffset,
                         _rctCtrl.y + yOffset - ty, _color, _p_FontText);
}

void CheckBoxGfx::RedrawButton(SDL_Surface* pScreen,
                               SDL_Surface* pScene_background) {
    if (pScene_background) {
        SDL_BlitSurface(pScene_background, &_rctCtrl, pScreen, &_rctCtrl);
    }
    DrawButton(pScreen);
}
