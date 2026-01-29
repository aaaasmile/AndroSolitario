#ifndef MENU_MGR__H
#define MENU_MGR__H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "ErrorInfo.h"
#include "Traits.h"

class Languages;
class LabelLinkGfx;
class LabelGfx;
class GameSettings;
class MusicManager;

using namespace traits;

class MenuMgr {
   public:
    enum { MYIDLABELURL = 0 };

    MenuMgr();
    virtual ~MenuMgr();
    LPErrInApp Initialize(SDL_Surface* pScreen, UpdateScreenCb& fnUpdateScreen,
                          MenuDelegator& menuDelegator);
    LPErrInApp HandleRootMenuEvent(SDL_Event* pEvent,
                                   const SDL_Point& targetPos);
    LPErrInApp HandleRootMenuIterate();
    void SetBackground(SDL_Surface* pVal) { _p_SceneBackground = pVal; }
    void UpdateScreen(SDL_Surface* pScreen);

   private:
    LPErrInApp drawMenuText(SDL_Surface* psurf, const char* text, int x, int y,
                            SDL_Color& color, TTF_Font* customfont);
    LPErrInApp drawStaticScene();
    LPErrInApp drawMenuTextList();
    LPErrInApp rootMenuNext();
    void updateTextureAsFlipScreen();

   private:
    MenuDelegator _menuDlgt;
    TTF_Font* _p_fontDejBoldBig;
    TTF_Font* _p_fontDejSmall;
    TTF_Font* _p_fontDejUnderscoreSmall;
    SDL_Surface* _p_Screen;
    SDL_Surface* _p_ScreenBackbuffer;
    SDL_Surface* _p_MenuBox;
    SDL_Surface* _p_SceneBackground;
    UpdateScreenCb _fnUpdateScreen;
    MenuItemEnum _focusedMenuItem;
    MenuItemEnum _prevFocusedMenuItem;
    int _box_Y;
    int _box_X;
    int _hBar;
    int _screenW;
    int _screenH;
    SDL_Rect _rctPanelRedBox;
    LabelGfx* _p_LabelVersion;
    LabelLinkGfx* _p_homeUrl;
    GameSettings* _p_GameSettings;
    MusicManager* _p_MusicManager;
    bool _ignoreMouseEvent;
};

#endif
