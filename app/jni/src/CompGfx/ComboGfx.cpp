#include "ComboGfx.h"

#include "GfxUtil.h"

static const char* LP_PLUS = "+";
static const char* LP_MINUS = "-";

ComboGfx::ComboGfx() {
    _visibleState = INVISIBLE;
    _p_fontText = 0;
    _butID = 0;
    _enabled = true;
    _p_surfBar = 0;
    _currDataIndex = 0;
    _p_surfBoxSel = 0;
    _p_surfBoxUNSel = 0;
}

ComboGfx::~ComboGfx() {
    if (_p_surfBar) {
        SDL_FreeSurface(_p_surfBar);
        _p_surfBar = NULL;
    }
    if (_p_surfBoxSel) {
        SDL_FreeSurface(_p_surfBoxSel);
        _p_surfBoxSel = NULL;
    }
    if (_p_surfBoxUNSel) {
        SDL_FreeSurface(_p_surfBoxUNSel);
        _p_surfBoxUNSel = NULL;
    }
}

void ComboGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                          TTF_Font* pFont, int iButID,
                          SDL_Renderer* psdlRenderer, ClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctCtrl = *pRect;
    _p_sdlRenderer = psdlRenderer;

    int boxIncW = 20;
    int boxIncH = _rctCtrl.h;  //_rctCtrl.h / 2;
    _rctText.x = _rctCtrl.x + boxIncW;
    _rctText.y = _rctCtrl.y;
    _rctText.w = _rctCtrl.w - 2 * boxIncW;
    _rctText.h = _rctCtrl.h;

    _rctBoxUp.x = _rctText.x - boxIncW;  //_rctText.x + _rctText.w;
    _rctBoxUp.y = _rctText.y;
    _rctBoxUp.w = boxIncW;
    _rctBoxUp.h = boxIncH;

    _rctBoxDown.x = _rctBoxUp.x + _rctText.w + boxIncW;  //_rctBoxUp.x;
    _rctBoxDown.y = _rctBoxUp.y;  //_rctBoxUp.y + boxIncH;
    _rctBoxDown.w = boxIncW;
    _rctBoxDown.h = boxIncH;

    _p_surfBar = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctCtrl.w, _rctCtrl.h, 32,
                                      0, 0, 0, 0);
    SDL_FillRect(_p_surfBar, NULL,
                 SDL_MapRGBA(pScreen->format, 255, 128, 30, 0));
    SDL_SetSurfaceBlendMode(_p_surfBar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfBar, 127);

    _p_surfBoxSel = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctBoxUp.w,
                                         _rctBoxUp.h, 32, 0, 0, 0, 0);
    SDL_FillRect(_p_surfBoxSel, NULL,
                 SDL_MapRGBA(pScreen->format, 200, 200, 130, 0));
    SDL_SetSurfaceBlendMode(_p_surfBoxSel, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfBoxSel, 127);

    _p_surfBoxUNSel = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctBoxUp.w,
                                           _rctBoxUp.h, 32, 0, 0, 0, 0);
    SDL_FillRect(_p_surfBoxUNSel, NULL,
                 SDL_MapRGBA(pScreen->format, 255, 128, 30, 0));
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

void ComboGfx::MouseMove(SDL_Event& event, SDL_Surface* pScreen,
                         SDL_Texture* pScene_background,
                         SDL_Texture* pScreenTexture) {
    if (_visibleState == VISIBLE && _enabled) {
        if (event.motion.x >= _rctCtrl.x &&
            event.motion.x <= _rctCtrl.x + _rctCtrl.w &&
            event.motion.y >= _rctCtrl.y &&
            event.motion.y <= _rctCtrl.y + _rctCtrl.h) {
            // mouse inner button
            _color = GFX_UTIL_COLOR::Orange;
            RedrawButton(pScreen, pScene_background, pScreenTexture);
        } else {
            // mouse outside
            if (_color.r == GFX_UTIL_COLOR::Orange.r &&
                _color.g == GFX_UTIL_COLOR::Orange.g &&
                _color.b == GFX_UTIL_COLOR::Orange.b) {
                // button was selected
                _color = GFX_UTIL_COLOR::White;
                RedrawButton(pScreen, pScene_background, pScreenTexture);
            }
        }
    }
}

void ComboGfx::MouseUp(SDL_Event& event) {
    if (_visibleState == VISIBLE && _enabled) {
        int mx = event.motion.x;
        int my = event.motion.y;
        if (mx >= _rctBoxUp.x && mx <= _rctBoxUp.x + _rctBoxUp.w &&
            my >= _rctBoxUp.y && my <= _rctBoxUp.y + _rctBoxUp.h) {
            // mouse on up box
            _currDataIndex++;
            if (_currDataIndex >= _vctDataStrings.size()) {
                // circular buffer
                _currDataIndex = 0;
            }
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)
                    ->Click(_fncbClickEvent.self, _currDataIndex);

        } else if (mx >= _rctBoxDown.x && mx <= _rctBoxDown.x + _rctBoxDown.w &&
                   my >= _rctBoxDown.y && my <= _rctBoxDown.y + _rctBoxDown.h) {
            // mouse on down box
            if (_currDataIndex == 0) {
                // circular buffer
                _currDataIndex = _vctDataStrings.size() - 1;
            } else {
                _currDataIndex--;
            }
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)
                    ->Click(_fncbClickEvent.self, _currDataIndex);
        }
    }
}

void ComboGfx::DrawButton(SDL_Surface* pScreen) {
    if (_visibleState == INVISIBLE) {
        return;
    }
    bool upBoxSelected = false;
    bool downBoxSelected = false;
    if (_enabled) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        if (mx >= _rctBoxUp.x && mx <= _rctBoxUp.x + _rctBoxUp.w &&
            my >= _rctBoxUp.y && my <= _rctBoxUp.y + _rctBoxUp.h) {
            upBoxSelected = true;
        } else if (mx >= _rctBoxDown.x && mx <= _rctBoxDown.x + _rctBoxDown.w &&
                   my >= _rctBoxDown.y && my <= _rctBoxDown.y + _rctBoxDown.h) {
            downBoxSelected = true;
        }

        // background on up/down boxes
        if (upBoxSelected) {
            GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctBoxUp.w,
                                         _rctBoxUp.h, _rctBoxUp.x, _rctBoxUp.y,
                                         _p_surfBoxSel);
        } else {
            GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctBoxUp.w,
                                         _rctBoxUp.h, _rctBoxUp.x, _rctBoxUp.y,
                                         _p_surfBoxUNSel);
        }
        if (downBoxSelected) {
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
        TTF_SizeText(_p_fontText, _buttonText.c_str(), &tx, &ty);
        int iXOffSet = (_rctText.w - tx) / 2;
        if (iXOffSet < 0) {
            iXOffSet = 1;
        }
        int iYOffset = (_rctText.h - ty) / 2;
        _buttonText = _vctDataStrings[_currDataIndex];
        GFX_UTIL::DrawString(pScreen, _buttonText.c_str(),
                             _rctText.x + iXOffSet, _rctText.y + iYOffset,
                             GFX_UTIL_COLOR::White, _p_fontText);

        // draw text upper box
        TTF_SizeText(_p_fontText, LP_PLUS, &tx, &ty);
        iXOffSet = (_rctBoxUp.w - tx) / 2;
        if (iXOffSet < 0) {
            iXOffSet = 1;
        }
        iYOffset = 0;
        if (upBoxSelected) {
            _color = GFX_UTIL_COLOR::Orange;
        } else {
            _color = GFX_UTIL_COLOR::White;
        }
        GFX_UTIL::DrawString(pScreen, LP_PLUS, _rctBoxUp.x + iXOffSet,
                             _rctBoxUp.y + iYOffset, _color, _p_fontText);

        // draw text down box
        TTF_SizeText(_p_fontText, LP_MINUS, &tx, &ty);
        iXOffSet = (_rctBoxDown.w - tx) / 2;
        if (iXOffSet < 0) {
            iXOffSet = 1;
        }
        if (downBoxSelected) {
            _color = GFX_UTIL_COLOR::Orange;
        } else {
            _color = GFX_UTIL_COLOR::White;
        }
        GFX_UTIL::DrawString(pScreen, LP_MINUS, _rctBoxDown.x + iXOffSet,
                             _rctBoxDown.y + iYOffset, _color, _p_fontText);

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

void ComboGfx::RedrawButton(SDL_Surface* pScreen,
                            SDL_Texture* pScene_background,
                            SDL_Texture* pScreenTexture) {
    if (pScene_background) {
        SDL_RenderCopy(_p_sdlRenderer, pScene_background, NULL, NULL);
    }
    DrawButton(pScreen);
    SDL_RenderCopy(_p_sdlRenderer, pScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);
}
