#ifndef APPGFX__H
#define APPGFX__H

#ifdef _WINDOWS
#pragma warning(disable : 4786)
#endif

#include <SDL3/SDL.h>

#include <stack>
#include <string>

#include "ErrorInfo.h"
// #include "GameGfx/SolitarioGfx.h"
#include "GameSettings.h"
#include "Languages.h"
#include "Traits.h"
#include "WinTypeGlobal.h"

class MusicManager;
class HighScore;
class MenuMgr;
class CreditsView;
class OptionsGfx;
class GameSelector;
class GameHelp;

using namespace traits;

class AppGfx {
   public:
    AppGfx();
    ~AppGfx();

    LPErrInApp Init();
    LPErrInApp MainLoopEvent(SDL_Event* pEvent, SDL_AppResult& res);
    LPErrInApp MainLoopIterate();
    std::string GetPlayerName() { return _p_GameSettings->PlayerName; }
    void SetPlayerName(std::string strVal) {
        _p_GameSettings->PlayerName = strVal;
    }
    void ParseCmdLine(int argc, char* argv[], SDL_AppResult& res);
    LPErrInApp LeaveMenu();
    LPErrInApp EnterMenu(MenuItemEnum menuItem);
    LPErrInApp SettingsChanged(bool backGroundChanged, bool languageChanged);
    LPErrInApp ChangeSceneBackground(SDL_Surface** ppSceneBackground);
    void UpdateScreen(SDL_Surface* pScreen);
    void RenderTexture(SDL_Texture* pScreenTexture);

   private:
    LPErrInApp startGameLoop();
    LPErrInApp createWindow();
    LPErrInApp selectLayout(int w, int h);
    LPErrInApp createScreenLayout();
    void terminate();
    LPErrInApp loadProfile();
    void updateScreenTexture();
    LPErrInApp showHelp();
    LPErrInApp showCredits();
    LPErrInApp showHighScore();
    LPErrInApp showGeneralOptions();
    void backToMenuRootWithMusic();
    void backToMenuRootSameMusic();
    void transformMouseToTarget(int windowX, int windowY,
                                SDL_Point* pTargetPos);

    MenuDelegator prepMenuDelegator();
    OptionDelegator prepOptionDelegator();
    UpdateScreenCb prepScreenUpdater();
    UpdateHighScoreCb prepHighScoreCb();

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
    // SolitarioGfx* _p_SolitarioGfx;
    GameSelector* _p_GameSelector;
    MusicManager* _p_MusicManager;
    HighScore* _p_HighScore;
    MenuMgr* _p_MenuMgr;
    CreditsView* _p_CreditsView;
    OptionsGfx* _p_OptGfx;
    GameHelp* _p_GameHelp;

    int _Bpp;
    bool _fullScreen;
    std::stack<MenuItemEnum> _histMenu;
    Uint64 _lastMainLoopticks;
    GameGfxCb _fnGameGfxCb;
};

#endif
