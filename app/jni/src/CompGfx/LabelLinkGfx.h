#ifndef CLABELLINKGFX__H
#define CLABELLINKGFX__H

#if _MSC_VER > 1000
#pragma once
#endif

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Traits.h"
#include "WinTypeGlobal.h"

using namespace traits;

class LabelLinkGfx {
public:
    enum eSate { VISIBLE, INVISIBLE, PRESSED };

    LabelLinkGfx();
    virtual ~LabelLinkGfx();

    void Initialize(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont,
                    int iButID, ClickCb& fncbClickEvent);
    void SetWindowText(LPCSTR strCaption) { _ctrlText = strCaption; }
    void MouseMove(SDL_Event& event);
    void MouseUp(SDL_Event& event);
    virtual void Draw(SDL_Surface* pScreen);
    void EnableWindow(bool bVal) { _isEnabled = bVal; }
    void SetState(eSate eVal);
    void SetUrl(LPCSTR lpszUrl) { _url = lpszUrl; }
    int PosX() { return _rctCtrl.x; }
    int PosY() { return _rctCtrl.y; }

protected:
    eSate _stateGfx;
    SDL_Rect _rctCtrl;
    STRING _ctrlText;
    bool _isEnabled;
    ClickCb _fncbClickEvent;
    SDL_Surface* _p_SurfBar;
    TTF_Font* _p_FontText;
    SDL_Color _color;
    int _CtrlID;
    STRING _url;
    bool _isDirty;
    bool _mouseOuside;
};

#endif
