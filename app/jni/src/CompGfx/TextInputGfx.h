#ifndef TEXTINPUTGFX__H
#define TEXTINPUTGFX__H

#if _MSC_VER > 1000
#pragma once
#endif
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "WinTypeGlobal.h"

class TextInputGfx {
   public:
    enum VisbleState { VISIBLE, INVISIBLE, SELECTED };
    TextInputGfx();
    ~TextInputGfx();

    void Initialize(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont, SDL_Window* pWindow);
    const std::string& GetText() const { return _text; }
    void SetText(const std::string& newText) { _text = newText; }
    bool GetHasFocus() const { return _hasFocus; }
    void HandleEvent(SDL_Event* pEvent);
    void SetVisibleState(VisbleState eVal) { _visibleState = eVal; }
    void DrawCtrl(SDL_Surface* pScreen);
    void Update();
    int PosX() { return _rctCtrl.x; }
    int PosY() { return _rctCtrl.y; }
    int Height() { return _rctCtrl.h; }

   private:
    bool _hasFocus;
    std::string _text;
    VisbleState _visibleState;
    SDL_Surface* _p_ctrlSurface;
    TTF_Font* _p_fontText;
    SDL_Rect _rctCtrl;
    bool _cursorVisible;
    Uint64 _lastBlinkTime;
    SDL_Window* _p_Window;
};

#endif