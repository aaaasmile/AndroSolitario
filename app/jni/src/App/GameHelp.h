#ifndef GAMEHELP_H
#define GAMEHELP_H

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#include "ErrorInfo.h"
#include "Traits.h"

using namespace traits;

class GameSettings;
// class UpdateScreenCb; // Removed, relying on Traits.h

enum class HelpItemType {
    TEXT,
    IMAGE,
    NEW_LINE,
    PARAGRAPH_BREAK
};

struct HelpItem {
    HelpItemType type;
    std::string text;
    std::string imagePath;  // Relative path
};

struct HelpPage {
    std::string title;
    std::vector<HelpItem> items;
};

class GameHelp {
   public:
    GameHelp();
    ~GameHelp();

    LPErrInApp Init();
    LPErrInApp Show(SDL_Surface* pScreen, UpdateScreenCb& fnUpdateScreen);
    LPErrInApp HandleEvent(SDL_Event* pEvent);
    LPErrInApp HandleIterate(bool& done);
    bool IsOngoing();
    void Reset();

   private:
    void buildPages();
    void renderCurrentPage();
    void drawJustifiedText(const std::string& text, int& y, int leftMargin,
                           int rightMargin);
    void drawPageContent();

   private:
    SDL_Surface* _p_Screen;
    GameSettings* _p_GameSettings;
    UpdateScreenCb _fnUpdateScreen;
    std::vector<HelpPage> _pages;
    int _currentPageIndex;
    bool _isInitialized;
    bool _isShown;

    // Layout constants
    const int MARGIN_X = 20;
    const int MARGIN_TOP = 60;
    const int LINE_HEIGHT_FACTOR = 30;  // approx pixels per line
};

#endif  // GAMEHELP_H