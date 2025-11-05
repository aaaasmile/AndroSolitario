#ifndef _CBUTTON_GFX_H
#define _CBUTTON_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Traits.h"
#include "WinTypeGlobal.h"

using namespace traits;
class GameSettings;

class ButtonGfx {
   public:
    enum VisbleState { VISIBLE, INVISIBLE, DISABLED };
    enum ButtonType { TEXT_BUTTON, SYMBOL_BT };
    enum MouseState { OUTSIDE, INSIDE };

    ButtonGfx();
    ~ButtonGfx();

    void Initialize(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont,
                    int iButID, ClickCb& fncbClickEvent);
    void InitializeAsSymbol(SDL_Rect* pRect, SDL_Surface* pScreen,
                                       TTF_Font* pFont, int iButID,
                                       ClickCb& fncbClickEvent);
    void SetButtonText(LPCSTR text) {
        _buttonText = text;
    }
    bool MouseMove(SDL_Event& event);
    bool MouseUp(SDL_Event& event);
    bool MouseDown(SDL_Event& event);
    void FingerDown(SDL_Event& event);
    void DrawButton(SDL_Surface* pScreen);
    void Enable(bool bVal) { _enabled = bVal; }
    void SetVisibleState(VisbleState eVal) { _visibleState = eVal; }
    void GetRect(SDL_Rect& rect) { rect = _rctButton; }

   private:
    SDL_Surface* _p_buttonSurface;
    TTF_Font* _p_fontText;
    GameSettings* _p_GameSettings;
    int _butID;
    bool _mouseIsDown;
    ButtonType _buttonType;
    MouseState _mouseState;
    VisbleState _visibleState;
    SDL_Rect _rctButton;
    STRING _buttonText;
    bool _enabled;
    ClickCb _fncbClickEvent;
};

#endif
