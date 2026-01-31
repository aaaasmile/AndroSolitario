
#ifndef CLABELGFX__H_
#define CLABELGFX__H_

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Config.h"

class LabelGfx {
   public:
    enum SateGfx { VISIBLE, INVISIBLE, PRESSED };

    LabelGfx();
    virtual ~LabelGfx();

    void Initialize(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont);
    void SetWindowText(LPCSTR strCaption) { _CtrlText = strCaption; }
    void Draw(SDL_Surface* pScreen);
    void EnableWindow(bool bVal) { _isEnabled = bVal; }
    void SetState(SateGfx eVal);

   private:
    SateGfx _StateGfx;
    SDL_Rect _rctCtrl;
    STRING _CtrlText;
    bool _isEnabled;
    SDL_Surface* _p_Surf_Bar;
    TTF_Font* _p_FontText;
    SDL_Color _color;
};

#endif
