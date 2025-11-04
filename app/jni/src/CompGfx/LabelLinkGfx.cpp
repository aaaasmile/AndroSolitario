#include "LabelLinkGfx.h"

#include "GfxUtil.h"

LabelLinkGfx::LabelLinkGfx() {
    _stateGfx = INVISIBLE;
    _p_FontText = 0;
    _CtrlID = 0;
    _isEnabled = true;
    _p_SurfBar = 0;
    _isDirty = true;
}

LabelLinkGfx::~LabelLinkGfx() {
    if (_p_SurfBar) {
        SDL_DestroySurface(_p_SurfBar);
        _p_SurfBar = NULL;
    }
}

void LabelLinkGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                              TTF_Font* pFont, int iButID,
                              ClickCb& fncbClickEvent) {
    _fncbClickEvent = fncbClickEvent;
    _rctCtrl = *pRect;

    // black bar surface
    // _p_SurfBar = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctCtrl.w, _rctCtrl.h,
    // 32,
    //                                   0, 0, 0, 0); SDL 2
    _p_SurfBar =
        GFX_UTIL::SDL_CreateRGBSurface(_rctCtrl.w, _rctCtrl.h, 32, 0, 0, 0, 0);

    // SDL_FillRect(_p_SurfBar, NULL, SDL_MapRGBA(pScreen->format, 255, 0, 0,
    // 0)); SDL 2
    SDL_FillSurfaceRect(_p_SurfBar, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 0, 0));

    SDL_SetSurfaceBlendMode(_p_SurfBar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_SurfBar, 127);

    _p_FontText = pFont;

    _color = GFX_UTIL_COLOR::White;
    _CtrlID = iButID;
}

void LabelLinkGfx::SetState(eSate eVal) {
    eSate eOldState = _stateGfx;
    _stateGfx = eVal;
    if (eOldState != _stateGfx && _stateGfx == VISIBLE) {
        _color = GFX_UTIL_COLOR::White;
    }
    _isDirty = true;
}

void LabelLinkGfx::MouseMove(SDL_Event& event) {
    if (_stateGfx == VISIBLE && _isEnabled) {
        if (event.motion.x >= _rctCtrl.x &&
            event.motion.x <= _rctCtrl.x + _rctCtrl.w &&
            event.motion.y >= _rctCtrl.y &&
            event.motion.y <= _rctCtrl.y + _rctCtrl.h) {
            // mouse inner button
            if (_mouseOuside) {
                _isDirty = true;
                _mouseOuside = false;
            }
        } else {
            // mouse outside
            if (!_mouseOuside) {
                _isDirty = true;
                _mouseOuside = true;
            }
        }
    }
}

void LabelLinkGfx::MouseUp(SDL_Event& event) {
    if (_stateGfx == VISIBLE && _isEnabled) {
        if (event.motion.x >= _rctCtrl.x &&
            event.motion.x <= _rctCtrl.x + _rctCtrl.w &&
            event.motion.y >= _rctCtrl.y &&
            event.motion.y <= _rctCtrl.y + _rctCtrl.h) {
            // mouse go up inner the button
            // send a click event
            if ((_fncbClickEvent.tc) != NULL)
                (_fncbClickEvent.tc)->Click(_fncbClickEvent.self, _CtrlID);
            if (_url.length()) {
#ifdef WIN32
                const char* cmd = NULL;
                char cmdpath[PATH_MAX];

                cmd = "start";
                snprintf(cmdpath, sizeof(cmdpath), "%s %s", cmd, _url.c_str());
                system(cmdpath);
#endif
            }
        }
    }
}

void LabelLinkGfx::Draw(SDL_Surface* pScreen) {
    if (!_isDirty) {
        return;
    }
    if (_stateGfx != INVISIBLE) {
        if (_isEnabled) {
            // begin stuff mouse
            int mx, my;
            float fmx, fmy;
            SDL_GetMouseState(&fmx, &fmy);
            mx = (int)fmx;
            my = (int)fmy;
            if (mx >= _rctCtrl.x && mx <= _rctCtrl.x + _rctCtrl.w &&
                my >= _rctCtrl.y && my <= _rctCtrl.y + _rctCtrl.h) {
                // mouse on button
                _color = GFX_UTIL_COLOR::Orange;
            } else {
                _color = GFX_UTIL_COLOR::White;
            }
            // end stuff mouse

            int tx, ty;
            // TTF_SizeText(_p_FontText, _ctrlText.c_str(), &tx, &ty); SDL2
            TTF_GetStringSize(_p_FontText, _ctrlText.c_str(), 0, &tx, &ty);
            int iXOffSet = (_rctCtrl.w - tx) / 2;
            if (iXOffSet < 0) {
                iXOffSet = 1;
            }
            int iYOffset = (_rctCtrl.h - ty) / 2;

            iYOffset = 0;
            iXOffSet = 0;
            GFX_UTIL::DrawString(pScreen, _ctrlText.c_str(),
                                 _rctCtrl.x + iXOffSet, _rctCtrl.y + iYOffset,
                                 _color, _p_FontText);
            _isDirty = false;
        }
    }
}
