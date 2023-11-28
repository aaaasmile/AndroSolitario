#ifndef APPGFX__H
#define APPGFX__H

#ifdef _WINDOWS
#pragma warning(disable : 4786)
#endif

#include <SDL.h>
#include <SDL_ttf.h>

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
    Languages* GetLanguageMan() { return &_Languages; }
    void ParseCmdLine(int argc, char* argv[]);
    TTF_Font* GetFontVera() { return _p_fontVera; }  // text small
    TTF_Font* GetFontAriblk() {
        return _p_fontAriblk;
    }  // text for big command on menu
    void LeaveMenu();
    void SetNextMenu(MenuItemEnum menuItem) { _histMenu.push(menuItem); }
    LPErrInApp SettingsChanged(bool backGroundChanged, bool languageChanged);

private:
    int waitKeyLoop();
    LPErrInApp startGameLoop();
    LPErrInApp createWindow();
    void terminate();
    LPErrInApp loadProfile();
    LPErrInApp writeProfile();
    void usage(int errOut, char* cmd);
    bool parseScreenSize(LPCSTR strOpz);
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
    TTF_Font* _p_fontVera;
    TTF_Font* _p_fontAriblk;
    GameSettings* _p_GameSettings;
    SolitarioGfx* _p_SolitarioGfx;
    MusicManager* _p_MusicManager;
    HighScore* _p_HighScore;
    Languages _Languages;

    int _iScreenW;
    int _iScreenH;
    int _iBpp;

    bool _bFullScreen;
    bool _bOverride;
    bool _backGroundChanged;

    std::stack<MenuItemEnum> _histMenu;
};

#endif
