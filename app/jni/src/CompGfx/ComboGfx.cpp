#include "ComboGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

static const char* LP_PLUS = "‹";
static const char* LP_MINUS = "›";

ComboGfx::ComboGfx() {
    _visibleState = INVISIBLE;
    _p_fontText = 0;
    _butID = 0;
    _enabled = true;
    _p_surfBar = 0;
    _currDataIndex = 0;
    _p_surfBoxSel = 0;
    _p_surfBoxUNSel = 0;
    _p_GameSettings = GameSettings::GetSettings();
    _fnUpdateScreen.tc = NULL;
    _fnUpdateScreen.self = NULL;
}

ComboGfx::~ComboGfx() {
    if (_p_surfBar) {
        SDL_DestroySurface(_p_surfBar);
        _p_surfBar = NULL;
    }
    if (_p_surfBoxSel) {
        SDL_DestroySurface(_p_surfBoxSel);
        _p_surfBoxSel = NULL;
    }
    if (_p_surfBoxUNSel) {
        SDL_DestroySurface(_p_surfBoxUNSel);
        _p_surfBoxUNSel = NULL;
    }
}

void ComboGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                          TTF_Font* pFont, int iButID,
                          UpdateScreenCb& fnUpdateScreen,
                          ClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctCtrl = *pRect;
    _fnUpdateScreen = fnUpdateScreen;
   
    int boxIncW = 34;
    int boxIncH = _rctCtrl.h;
    _rctText.x = _rctCtrl.x + boxIncW;
    _rctText.y = _rctCtrl.y;
    _rctText.w = _rctCtrl.w - 2 * boxIncW;
    _rctText.h = _rctCtrl.h;

    _rctBoxUp.x = _rctText.x - boxIncW;
    _rctBoxUp.y = _rctText.y;
    _rctBoxUp.w = boxIncW;
    _rctBoxUp.h = boxIncH;

    _rctBoxDown.x = _rctBoxUp.x + _rctText.w + boxIncW;
    _rctBoxDown.y = _rctBoxUp.y;
    _rctBoxDown.w = boxIncW;
    _rctBoxDown.h = boxIncH;

    _p_surfBar =
        GFX_UTIL::SDL_CreateRGBSurface(_rctCtrl.w, _rctCtrl.h, 32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_surfBar, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 128, 30));

    SDL_SetSurfaceBlendMode(_p_surfBar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfBar, 127);

    _p_surfBoxSel = GFX_UTIL::SDL_CreateRGBSurface(_rctBoxUp.w, _rctBoxUp.h, 32,
                                                   0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_surfBoxSel, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 200, 200, 130));

    SDL_SetSurfaceBlendMode(_p_surfBoxSel, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfBoxSel, 127);

    _p_surfBoxUNSel = GFX_UTIL::SDL_CreateRGBSurface(_rctBoxUp.w, _rctBoxUp.h,
                                                     32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_surfBoxUNSel, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 128, 30));

    SDL_SetSurfaceBlendMode(_p_surfBoxUNSel, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfBoxUNSel, 127);

    _p_fontText = pFont;

    _color = GFX_UTIL_COLOR::White;
    _butID = iButID;
    _currDataIndex = 0;
    _vctDataStrings.clear();
}

void ComboGfx::AddLineText(LPCSTR strLine) {
    _vctDataStrings.push_back(strLine);
}

void ComboGfx::SelectIndex(int iIndex) {
    if (iIndex >= 0 && iIndex < _vctDataStrings.size()) {
        _currDataIndex = iIndex;
    }
}

void ComboGfx::SetVisibleState(VisbleState eVal) {
    VisbleState eOldState = _visibleState;
    _visibleState = eVal;
    if (eOldState != _visibleState && _visibleState == VISIBLE) {
        _color = GFX_UTIL_COLOR::White;
    }
}

void ComboGfx::MouseMove(SDL_Event* pEvent, const SDL_Point& targetPos) {
    if (_visibleState == VISIBLE && _enabled) {
        if (targetPos.x >= _rctCtrl.x &&
            targetPos.x <= _rctCtrl.x + _rctCtrl.w &&
            targetPos.y >= _rctCtrl.y &&
            targetPos.y <= _rctCtrl.y + _rctCtrl.h) {
            int mx = targetPos.x;
            int my = targetPos.y;
            if (mx >= _rctBoxUp.x && mx <= _rctBoxUp.x + _rctBoxUp.w &&
                my >= _rctBoxUp.y && my <= _rctBoxUp.y + _rctBoxUp.h) {
                _upBoxSelected = true;
            } else if (mx >= _rctBoxDown.x &&
                       mx <= _rctBoxDown.x + _rctBoxDown.w &&
                       my >= _rctBoxDown.y &&
                       my <= _rctBoxDown.y + _rctBoxDown.h) {
                _downBoxSelected = true;
            }
            _color = GFX_UTIL_COLOR::Orange;
        } else {
            if (_upBoxSelected || _downBoxSelected){
                _upBoxSelected = false;
                _downBoxSelected = false;
            }
        }
    }
}

static bool IsPointInsideBox(const SDL_Rect& rct, const SDL_Point& pt) {
    if (pt.x >= rct.x && pt.x <= rct.x + rct.w && pt.y >= rct.y &&
        pt.y <= rct.y + rct.h) {
        return true;
    }
    return false;
}

void ComboGfx::FingerDown(SDL_Event* pEvent) {
    SDL_Point pt;
    _p_GameSettings->GetTouchPoint(pEvent->tfinger, &pt);
    if (IsPointInsideBox(_rctBoxUp, pt)) {
        _currDataIndex++;
        if (_currDataIndex >= _vctDataStrings.size()) {
            _currDataIndex = 0;
        }
        if ((_fncbClickEvent.tc) != NULL)
            (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _currDataIndex);
    } else if (IsPointInsideBox(_rctBoxDown, pt)) {
        if (_currDataIndex == 0) {
            _currDataIndex = _vctDataStrings.size() - 1;
        } else {
            _currDataIndex--;
        }
        if ((_fncbClickEvent.tc) != NULL)
            (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _currDataIndex);
    }
}

void ComboGfx::MouseUp(SDL_Event* pEvent, const SDL_Point& targetPos) {
    if (_p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
        return;
    }
    if (_visibleState == VISIBLE && _enabled) {
        int mx = targetPos.x;
        int my = targetPos.y;
        if (mx >= _rctBoxUp.x && mx <= _rctBoxUp.x + _rctBoxUp.w &&
            my >= _rctBoxUp.y && my <= _rctBoxUp.y + _rctBoxUp.h) {
            _currDataIndex++;
            if (_currDataIndex >= _vctDataStrings.size()) {
                _currDataIndex = 0;
            }
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)
                    ->Click(_fncbClickEvent.self, _currDataIndex);

        } else if (mx >= _rctBoxDown.x && mx <= _rctBoxDown.x + _rctBoxDown.w &&
                   my >= _rctBoxDown.y && my <= _rctBoxDown.y + _rctBoxDown.h) {
            if (_currDataIndex == 0) {
                _currDataIndex = _vctDataStrings.size() - 1;
            } else {
                _currDataIndex--;
            }
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)
                    ->Click(_fncbClickEvent.self, _currDataIndex);
        }
    }
    // TRACE_DEBUG("[combo] Index %d, size %d \n", _currDataIndex,
    //             _vctDataStrings.size());
}

void ComboGfx::DrawButton(SDL_Surface* pScreen) {
    if (_visibleState == INVISIBLE) {
        return;
    }
    if (_enabled) {
        // background on up/down boxes
        if (_upBoxSelected) {
            GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctBoxUp.w,
                                         _rctBoxUp.h, _rctBoxUp.x, _rctBoxUp.y,
                                         _p_surfBoxSel);
        } else {
            GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctBoxUp.w,
                                         _rctBoxUp.h, _rctBoxUp.x, _rctBoxUp.y,
                                         _p_surfBoxUNSel);
        }
        if (_downBoxSelected) {
            GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctBoxDown.w,
                                         _rctBoxDown.h, _rctBoxDown.x,
                                         _rctBoxDown.y, _p_surfBoxSel);
        } else {
            GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctBoxDown.w,
                                         _rctBoxDown.h, _rctBoxDown.x,
                                         _rctBoxDown.y, _p_surfBoxUNSel);
        }

        // draw current selected text
        int tx, ty;
        TTF_GetStringSize(_p_fontText, _buttonText.c_str(), 0, &tx, &ty);
        int xOffset = (_rctText.w - tx) / 2;
        if (xOffset < 0) {
            xOffset = 1;
        }
        int yOffset = (_rctText.h - ty) / 2;
        _buttonText = _vctDataStrings[_currDataIndex];
        GFX_UTIL::DrawString(pScreen, _buttonText.c_str(), _rctText.x + xOffset,
                             _rctText.y + yOffset, GFX_UTIL_COLOR::White,
                             _p_fontText);

        // draw text upper box
        TTF_GetStringSize(_p_GameSettings->GetFontDjvMedium(), LP_PLUS, 0, &tx, &ty);

        xOffset = (_rctBoxUp.w - tx) / 2;
        if (xOffset < 0) {
            xOffset = 1;
        }
        yOffset = (_rctBoxUp.h - ty) / 2;
        if (yOffset < 0) {
            yOffset = 0;
        }
        if (_upBoxSelected) {
            _color = GFX_UTIL_COLOR::Orange;
        } else {
            _color = GFX_UTIL_COLOR::White;
        }
        GFX_UTIL::DrawString(pScreen, LP_PLUS, _rctBoxUp.x + xOffset,
                             _rctBoxUp.y + yOffset, _color, _p_GameSettings->GetFontDjvMedium());

        // draw text down box
        TTF_GetStringSize(_p_GameSettings->GetFontDjvMedium(), LP_MINUS, 0, &tx, &ty);

        xOffset = (_rctBoxDown.w - tx) / 2;
        if (xOffset < 0) {
            xOffset = 1;
        }
        if (_downBoxSelected) {
            _color = GFX_UTIL_COLOR::Orange;
        } else {
            _color = GFX_UTIL_COLOR::White;
        }
        GFX_UTIL::DrawString(pScreen, LP_MINUS, _rctBoxDown.x + xOffset,
                             _rctBoxDown.y + yOffset, _color, _p_GameSettings->GetFontDjvMedium());

        // draw borders
        GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 1, _rctCtrl.y - 1,
                           _rctCtrl.x + _rctCtrl.w + 1,
                           _rctCtrl.y + _rctCtrl.h + 1, GFX_UTIL_COLOR::Gray);
        GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 2, _rctCtrl.y - 2,
                           _rctCtrl.x + _rctCtrl.w + 2,
                           _rctCtrl.y + _rctCtrl.h + 2, GFX_UTIL_COLOR::Black);
        GFX_UTIL::DrawRect(pScreen, _rctCtrl.x, _rctCtrl.y,
                           _rctCtrl.x + _rctCtrl.w, _rctCtrl.y + _rctCtrl.h,
                           GFX_UTIL_COLOR::White);

        GFX_UTIL::DrawRect(pScreen, _rctBoxUp.x + 1, _rctBoxUp.y + 1,
                           _rctBoxUp.x + _rctBoxUp.w - 1,
                           _rctBoxUp.y + _rctBoxUp.h - 1,
                           GFX_UTIL_COLOR::Black);

        GFX_UTIL::DrawRect(pScreen, _rctBoxDown.x + 1, _rctBoxDown.y + 1,
                           _rctBoxDown.x + _rctBoxDown.w - 1,
                           _rctBoxDown.y + _rctBoxDown.h - 1,
                           GFX_UTIL_COLOR::Black);

        GFX_UTIL::DrawRect(pScreen, _rctBoxUp.x, _rctBoxUp.y,
                           _rctBoxUp.x + _rctBoxUp.w, _rctBoxUp.y + _rctBoxUp.h,
                           GFX_UTIL_COLOR::White);
        GFX_UTIL::DrawRect(pScreen, _rctBoxDown.x, _rctBoxDown.y,
                           _rctBoxDown.x + _rctBoxDown.w,
                           _rctBoxUp.y + _rctBoxDown.h, GFX_UTIL_COLOR::White);
    }
}
