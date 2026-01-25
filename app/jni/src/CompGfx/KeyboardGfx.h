#ifndef _KEYBOARD_GFX_H
#define _KEYBOARD_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>

#include "ButtonGfx.h"
#include "Traits.h"
#include "TypeGlobal.h"

using namespace traits;

class KeyboardGfx {
   public:
    enum VisbleState { VISIBLE, INVISIBLE };
    enum MouseState { OUTSIDE, INSIDE };
    enum SpecialChar {
        ID_BACKSPACE = 8,
        ID_RETURN = 13,
        ID_SPACE = 32,
        ID_SHIFT = 1000
    };

    KeyboardGfx();
    ~KeyboardGfx();

    void Show(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont,
              ClickKeyboardCb& fncbClickEvent);

    void HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos);
    void DrawCtrl(SDL_Surface* pScreen);
    void SetVisibleState(VisbleState eVal) { _visibleState = eVal; }
    void GetRect(SDL_Rect& rect) { rect = _rctCtrl; }
    void OnButtonClickImpl(int btID);
    void RefreshLabels();

   private:
    ClickCb prepClickCb();

    SDL_Surface* _p_keyboardSurface;
    TTF_Font* _p_fontText;
    VisbleState _visibleState;
    SDL_Rect _rctCtrl;
    ClickKeyboardCb _fncbKeyboardEvent;

    std::vector<ButtonGfx*> _buttons;
    bool _isShifted;
};

#endif
