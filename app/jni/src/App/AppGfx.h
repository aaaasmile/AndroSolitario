#ifndef APPGFX__H
#define APPGFX__H

#ifdef _WINDOWS
#pragma warning(disable : 4786)
#endif

#include <SDL3/SDL.h>

#include <stack>
#include <string>

#include "ErrorInfo.h"
#include "GameGfx/SolitarioGfx.h"
#include "Languages.h"
#include "Traits.h"
#include "WinTypeGlobal.h"

class MusicManager;
class HighScore;

using namespace traits;

class AppGfx {
public:
    AppGfx();
    ~AppGfx();

    LPErrInApp Init();
    LPErrInApp MainLoop();
    std::string GetPlayerName() { return _p_GameSettings->PlayerName; }
    void SetPlayerName(std::string strVal) {
        _p_GameSettings->PlayerName = strVal;
    }
    void ParseCmdLine(int argc, char* argv[]);
    void LeaveMenu();
    void SetNextMenu(MenuItemEnum menuItem) { _histMenu.push(menuItem); }
    LPErrInApp SettingsChanged(bool backGroundChanged, bool languageChanged);

private:
    LPErrInApp startGameLoop();
    LPErrInApp createWindow();
    void terminate();
    LPErrInApp loadProfile();
    LPErrInApp writeProfile();
    void updateScreenTexture();
    LPErrInApp showHelp();
    LPErrInApp showCredits();
    LPErrInApp showHighScore();
    LPErrInApp showOptionGeneral();

    MenuDelegator prepMenuDelegator();
    void clearBackground();
    LPErrInApp loadSceneBackground();

private:
    SDL_Surface* _p_Screen;
    SDL_Surface* _p_SceneBackground;
    SDL_Surface* _p_CreditTitle;
    SDL_Texture* _p_ScreenTexture;
    SDL_Window* _p_Window;
    SDL_Renderer* _p_sdlRenderer;
    GameSettings* _p_GameSettings;
    SolitarioGfx* _p_SolitarioGfx;
    MusicManager* _p_MusicManager;
    HighScore* _p_HighScore;
    

    int _screenW;
    int _screenH;
    int _Bpp;

    bool _fullScreen;
    bool _backGroundChanged;

    std::stack<MenuItemEnum> _histMenu;
};

#endif
