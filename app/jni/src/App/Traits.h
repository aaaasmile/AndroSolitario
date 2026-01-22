#ifndef TRAITS_FASTDELEGATE__H_
#define TRAITS_FASTDELEGATE__H_

#include <SDL3_ttf/SDL_ttf.h>

#include "ErrorInfo.h"
#include "Languages.h"
#include <vector>

enum MenuItemEnum {
    MENU_GAME = 0,
    MENU_OPTIONS = 1,
    MENU_CREDITS = 2,
    MENU_HELP = 3,
    MENU_HIGHSCORE = 4,
    QUIT = 5,
    MENU_ROOT = 6,
    NOTHING = 99
};

enum class HelpItemType { TEXT, IMAGE, NEW_LINE, PARAGRAPH_BREAK };
enum PageNav { NEXT, PREV, HOME };

struct HelpItem {
    HelpItemType Type;
    std::string Text;
    std::string ImagePath;
};

struct HelpPage {
    std::string Title;
    std::vector<HelpItem> Items;
};

namespace traits {
// trait for menu
typedef struct {
    LPErrInApp (*const LeaveMenu)(void* self);
    LPErrInApp (*const EnterMenu)(void* self, MenuItemEnum menuItem);
} VMenuDelegator, *LPVMenuDelegator;

typedef struct {
    VMenuDelegator const* tc;
    void* self;
} MenuDelegator, *LPMenuDelegator;

typedef struct {
    LPErrInApp (*const SettingsChanged)(void* self, bool backGroundChanged,
                                        bool languageChanged);
    LPErrInApp (*const ChangeSceneBackground)(void* self,
                                              SDL_Surface** ppSceneBackground);
} VOptionDelegator, *LPVOptionDelegator;

typedef struct {
    VOptionDelegator const* tc;
    void* self;
} OptionDelegator, *LPOptionDelegator;

// trait for button click
typedef struct {
    void (*const Click)(void* self, int btID);
} VClickCb, *LPVClickCb;

typedef struct {
    VClickCb const* tc;
    void* self;
} ClickCb, *LPClickCb;

// trait for keyboard click
typedef struct {
    void (*const ClickKey)(void* self, const char* text);
} VClickKeyboardCb, *LPVClickKeyboardCb;

typedef struct {
    VClickKeyboardCb const* tc;
    void* self;
} ClickKeyboardCb, *LPClickKeyboardCb;

// trait for checkbox click
typedef struct {
    void (*const Click)(void* self, bool state);
} VCheckboxClickCb, *LPVCheckboxClickCb;

typedef struct {
    VCheckboxClickCb const* tc;
    void* self;
} CheckboxClickCb, *LPCheckboxClickCb;

// trait for render

typedef struct {
    void (*const UpdateScreen)(void* self, SDL_Surface* pScreen);
    void (*const RenderTexture)(void* self, SDL_Texture* pTexture);
} VUpdateScreenCb, *LPVUpdateScreenCb;

typedef struct {
    VUpdateScreenCb const* tc;
    void* self;
} UpdateScreenCb, *LPUpdateScreenCb;

// trait for update high score
typedef struct {
    LPErrInApp (*const SaveScore)(void* self, int64_t score, int numCard);
} VUpdateHighScoreCb, *LPVUpdateHighScoreCb;

typedef struct {
    VUpdateHighScoreCb const* tc;
    void* self;
} UpdateHighScoreCb, *LPUpdateHighScoreCb;

// trait for GameGfx
typedef struct {
    LPErrInApp (*const HandleEvent)(void* self, SDL_Event* pEvent,
                                    const SDL_Point& targetPos);
    LPErrInApp (*const HandleIterate)(void* self, bool& done);
    LPErrInApp (*const Initialize)(void* self, SDL_Surface* pScreen,
                                   UpdateScreenCb& fnUpdateScreen,
                                   SDL_Window* pWindow, SDL_Surface* pSceneBackground,
                                   UpdateHighScoreCb& fnHighScore);
    LPErrInApp (*const Show)(void* self);
} VGameGfxCb, *LPVGameGfxCb;

typedef struct {
    VGameGfxCb const* tc;
    void* self;
} GameGfxCb, *LPGameGfxCb;

};  // namespace traits

#endif