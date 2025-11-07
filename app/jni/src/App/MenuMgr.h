#ifndef MENU_MGR__H
#define MENU_MGR__H

#if _MSC_VER > 1000
#pragma once
#endif

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "ErrorInfo.h"
#include "Traits.h"

class Languages;
class LabelLinkGfx;
class LabelGfx;

using namespace traits;

class MenuMgr {
   public:
    enum { MYIDLABELURL = 0 };

    MenuMgr();
    virtual ~MenuMgr();
    LPErrInApp Initialize(SDL_Surface* pScreen, SDL_Renderer* pRenderer,
                          SDL_Window* pWindow, MenuDelegator& menuDelegator);
    LPErrInApp HandleRootMenu();
    void SetBackground(SDL_Surface* pVal) { _p_SceneBackground = pVal; }

   private:
    LPErrInApp drawMenuText(SDL_Surface* psurf, const char* text, int x, int y,
                            SDL_Color& color, TTF_Font* customfont);
    LPErrInApp drawStaticScene();
    LPErrInApp drawMenuTextList();
    void rootMenuNext();
    void updateTextureAsFlipScreen();

   private:
    MenuDelegator _menuDlgt;
    TTF_Font* _p_fontAriblk;
    TTF_Font* _p_fontVera;
    TTF_Font* _p_fontVeraUnderscore;
    SDL_Window* _p_Window;
    SDL_Surface* _p_Screen;
    SDL_Surface* _p_ScreenBackbuffer;
    SDL_Surface* _p_MenuBox;
    SDL_Surface* _p_SceneBackground;
    SDL_Texture* _p_ScreenTexture;
    SDL_Renderer* _p_sdlRenderer;
    MenuItemEnum _focusedMenuItem;
    int _box_Y;
    int _box_X;
    int _hBar;
    int _screenW;
    int _screenH;
    SDL_Rect _rctPanelRedBox;
    LabelGfx* _p_LabelVersion;
    LabelLinkGfx* _p_homeUrl;
};

#endif
