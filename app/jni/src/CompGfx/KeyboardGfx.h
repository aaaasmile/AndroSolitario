#ifndef _KEYBOARD_GFX_H
#define _KEYBOARD_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Traits.h"
#include "WinTypeGlobal.h"
#include "ButtonGfx.h"
#include <vector>

using namespace traits;

class KeyboardGfx {
   public:
    enum VisbleState { VISIBLE, INVISIBLE };
    enum MouseState { OUTSIDE, INSIDE };

    KeyboardGfx();
    ~KeyboardGfx();

    void Show(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont, ClickKeyboardCb& fncbClickEvent);

    void HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos);
    void DrawCtrl(SDL_Surface* pScreen);
    void SetVisibleState(VisbleState eVal) { _visibleState = eVal; }
    void GetRect(SDL_Rect& rect) { rect = _rctCtrl; }
    void OnButtonClickImpl(int btID);

   private:
    ClickCb prepClickCb();

    SDL_Surface* _p_keyboardSurface;
    TTF_Font* _p_fontText;
    bool _mouseIsDown;
    MouseState _mouseState;
    VisbleState _visibleState;
    SDL_Rect _rctCtrl;
    ClickKeyboardCb _fncbKeyboardEvent;
    
    std::vector<ButtonGfx*> _buttons;
};

#endif
