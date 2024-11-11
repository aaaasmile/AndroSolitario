#ifndef TRAITS_FASTDELEGATE__H_
#define TRAITS_FASTDELEGATE__H_

#include <SDL3_ttf/SDL_ttf.h>

#include "ErrorInfo.h"
#include "Languages.h"

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

namespace traits {
    // trait for menu
    typedef struct {
        TTF_Font* (*const GetFontVera)(void* self);
        TTF_Font* (*const GetFontAriblk)(void* self);
        Languages* (*const GetLanguageMan)(void* self);
        void (*const LeaveMenu)(void* self);
        void (*const SetNextMenu)(void* self, MenuItemEnum menuItem);
        LPErrInApp (*const SettingsChanged)(void* self, bool backGroundChanged,
                                            bool languageChanged);
    } VMenuDelegator, *LPVMenuDelegator;

    typedef struct {
        VMenuDelegator const* tc;
        void* self;
    } MenuDelegator, *LPMenuDelegator;

    // trait for button click
    typedef struct {
        void (*const Click)(void* self, int btID);
    } VClickCb, *LPVClickCb;

    typedef struct {
        VClickCb const* tc;
        void* self;
    } ClickCb, *LPClickCb;

    // trait for checkbox click
    typedef struct {
        void (*const Click)(void* self, bool state);
    } VCheckboxClickCb, *LPVCheckboxClickCb;

    typedef struct {
        VCheckboxClickCb const* tc;
        void* self;
    } CheckboxClickCb, *LPCheckboxClickCb;
};

#endif