#include "ButtonGfx.h"

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
}

ButtonGfx::~ButtonGfx() {
    if (_p_buttonSurface) {
        SDL_FreeSurface(_p_buttonSurface);
        _p_buttonSurface = NULL;
    }
}

void ButtonGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                           TTF_Font* pFont, int iButID,
                           ClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctButton = *pRect;

    _p_buttonSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctButton.w,
                                            _rctButton.h, 32, 0, 0, 0, 0);
    SDL_FillRect(_p_buttonSurface, NULL,
                 SDL_MapRGBA(pScreen->format, 255, 0, 0, 0));
    SDL_SetSurfaceBlendMode(_p_buttonSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_buttonSurface, 127);  // SDL 2.0
    _p_fontText = pFont;
    _butID = iButID;
    _mouseIsDown = false;
    _buttonType = TEXT_BUTTON;
}

bool ButtonGfx::MouseMove(SDL_Event& event) {
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

bool ButtonGfx::MouseUp(SDL_Event& event) {
    if (_visibleState == VISIBLE && _enabled) {
        if (event.motion.x >= _rctButton.x &&
            event.motion.x <= _rctButton.x + _rctButton.w &&
            event.motion.y >= _rctButton.y &&
            event.motion.y <= _rctButton.y + _rctButton.h) {
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
    SDL_GetMouseState(&mx, &my);
    if (mx >= _rctButton.x && mx <= _rctButton.x + _rctButton.w &&
        my >= _rctButton.y && my <= _rctButton.y + _rctButton.h) {
        colorText = GFX_UTIL_COLOR::Orange;
        _mouseState = MouseState::INSIDE;
    } else {
        colorText = GFX_UTIL_COLOR::White;
        _mouseState = MouseState::OUTSIDE;
    }

    GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctButton.w, _rctButton.h,
                                 _rctButton.x, _rctButton.y, _p_buttonSurface);
    int tx, ty;
    TTF_SizeText(_p_fontText, _buttonText.c_str(), &tx, &ty);
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
