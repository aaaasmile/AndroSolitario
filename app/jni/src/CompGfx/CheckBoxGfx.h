#ifndef _CCHECKBOXGFX__H_
#define _CCHECKBOXGFX__H_

#include <SDL.h>
#include <SDL_ttf.h>

#include "Traits.h"
#include "WinTypeGlobal.h"

using namespace traits;

class CheckBoxGfx {
   public:
    CheckBoxGfx();
    virtual ~CheckBoxGfx();

   public:
    enum VisbleState {
        VISIBLE,
        INVISIBLE,
    };

    void Initialize(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont,
                    int iButID, CheckboxClickCb& fncbClickEvent);
    void SetWindowText(LPCSTR strCaption) { _buttonText = strCaption; }
    void MouseMove(SDL_Event& event, SDL_Surface* pScreen,
                   SDL_Surface* pScene_background);
    void MouseUp(SDL_Event& event);
    void DrawButton(SDL_Surface* pScreen);
    void EnableWindow(bool bVal) { _enabled = bVal; }
    void SetVisibleState(VisbleState eVal);
    void RedrawButton(SDL_Surface* pScreen, SDL_Surface* pScene_background);
    bool GetCheckState() { return _clicked; }
    void SetCheckState(bool bVal) { _clicked = bVal; }
    int PosX() { return _rctCtrl.x; }
    int PosY() { return _rctCtrl.y; }
    int Height() { return _rctCtrl.h; }

   private:
    VisbleState _visibleState;
    SDL_Rect _rctCtrl;
    STRING _buttonText;
    bool _enabled;
    CheckboxClickCb _fncbClickEvent;
    TTF_Font* _p_FontText;
    SDL_Color _color;
    bool _clicked;
    int _buttonID;
    SDL_Color _colorBorder;
};

#endif
