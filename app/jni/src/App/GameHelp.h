#ifndef GAMEHELP_H
#define GAMEHELP_H

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#include "ErrorInfo.h"
#include "Traits.h"

using namespace traits;

class GameSettings;
class ButtonGfx;

enum class HelpItemType { TEXT, IMAGE, NEW_LINE, PARAGRAPH_BREAK };
enum PageNav { NEXT, PREV, HOME };

struct HelpItem {
    HelpItemType type;
    std::string text;
    std::string imagePath;
};

struct HelpPage {
    std::string title;
    std::vector<HelpItem> items;
};

class GameHelp {
   public:
    GameHelp();
    ~GameHelp();

    LPErrInApp Show(SDL_Surface* pScreen, UpdateScreenCb& fnUpdateScreen,
                    SDL_Surface* pSceneBackground);
    LPErrInApp HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos);
    LPErrInApp HandleIterate(bool& done);
    bool IsOngoing() { return _isShown; }
    void NextPage();
    void PrevPage();
    void HomePage();

   private:
    void buildPages();
    ClickCb prepClickCb();
    LPErrInApp renderCurrentPage();
    LPErrInApp drawJustifiedText(const std::string& text, int& y,
                                 int leftMargin, int rightMargin);
    LPErrInApp drawPageContent();

   private:
    SDL_Surface* _p_Screen;
    SDL_Surface* _p_ShadowSrf = NULL;
    SDL_Surface* _p_Scene_background;
    SDL_Surface* _p_surfTextBackground;
    GameSettings* _p_GameSettings;
    UpdateScreenCb _fnUpdateScreen;
    std::vector<HelpPage> _pages;
    int _currentPageIndex;
    bool _isShown = false;
    bool _isDirty = false;
    bool _mouseDownRec = false;
    ButtonGfx* _p_buttonNext = NULL;
    ButtonGfx* _p_buttonPrev = NULL;
    ButtonGfx* _p_buttonHome = NULL;
    SDL_Rect _rctOptBox;

    // Layout constants
    const int MARGIN_X = 20;
    const int MARGIN_TOP = 60;
    const int LINE_HEIGHT_FACTOR = 30;  // approx pixels per line
};

#endif  // GAMEHELP_H