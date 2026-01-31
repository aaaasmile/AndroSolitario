#include "MenuMgr.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "CompGfx/LabelGfx.h"
#include "CompGfx/LabelLinkGfx.h"
#include "Config.h"
#include "ErrorInfo.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "MusicManager.h"

static const char* g_lpszMsgUrl = "Go to invido.it";
static const char* g_lpszVersion = VERSION;

static const SDL_Color g_color_on = {253, 252, 250};
static const SDL_Color g_color_off = {128, 128, 128};
static const SDL_Color g_color_white = {255, 255, 255};
static const SDL_Color g_color_ombre = {87, 87, 87, 50};
static const SDL_Color g_color_black = {0, 0, 0};
static const SDL_Color g_color_gray = {128, 128, 128};

MenuItemEnum nextMenu(MenuItemEnum currMenu);
MenuItemEnum previousMenu(MenuItemEnum currMenu);

typedef struct MenuItemBox {
    int StartY;
    int EndY;

    MenuItemEnum MenuItem;
    void SetY(int sy, int ey) {
        StartY = sy;
        EndY = ey;
    }
}* PMenuItemBox;

#if HASQUITMENU
static const int NumOfMenuItems = 6;
#else
static const int NumOfMenuItems = 5;
#endif
typedef class MenuItemBoxes {
    MenuItemBox _menuInfoBoxes[NumOfMenuItems];
    int _minX;
    int _maxX;
    int _minY;
    int _maxY;

   public:
    MenuItemBoxes() {
        _menuInfoBoxes[0] = {0, 0, MenuItemEnum::MENU_GAME};
        _menuInfoBoxes[1] = {0, 0, MenuItemEnum::MENU_OPTIONS};
        _menuInfoBoxes[2] = {0, 0, MenuItemEnum::MENU_CREDITS};
        _menuInfoBoxes[3] = {0, 0, MenuItemEnum::MENU_HELP};
        _menuInfoBoxes[4] = {0, 0, MenuItemEnum::MENU_HIGHSCORE};
#if HASQUITMENU
        _menuInfoBoxes[5] = {0, 0, MenuItemEnum::QUIT};
#endif
    }
    bool IsPointInside(const SDL_Point& tap, MenuItemBox& tapped) {
        tapped = {0, 0, MenuItemEnum::NOTHING};
        if (tap.x > _maxX || tap.x < _minX) {
            return false;
        }
        if (tap.y > _maxY || tap.y < _minY) {
            return false;
        }
        for (int i = 0; i < NumOfMenuItems; i++) {
            tapped.MenuItem = _menuInfoBoxes[i].MenuItem;
            if (tap.y >= _menuInfoBoxes[i].StartY &&
                tap.y <= _menuInfoBoxes[i].EndY) {
                return true;
            }
        }
        return false;
    }
    void SetBox(const SDL_Rect& rct) {
        _minX = rct.x;
        _minY = rct.y;
        _maxX = rct.x + rct.w;
        _maxY = rct.y + rct.h;
        TRACE_DEBUG("Menubox is %d -> %d, %d -> %d \n", _minX, _maxX, _minY,
                    _maxY);
    }
    void SetYInPos(int pos, int eY) {
        if (pos >= 1 && pos < NumOfMenuItems) {
            int sY = _menuInfoBoxes[pos - 1].EndY + 1;
            _menuInfoBoxes[pos].SetY(sY, eY);
        }
    }
    void SetPos0Y(int sY, int eY) { _menuInfoBoxes[0].SetY(sY, eY); }
    void drawMaxBox(SDL_Surface* pScreen) {
#if DEBUG_MENU_BORDER_BOX
        GFX_UTIL::DrawRect(pScreen, _minX, _minY, _maxX, _maxY, g_color_red);
#endif
    }
    void drawBorder(int i, SDL_Surface* pScreen) {
#if DEBUG_MENU_BORDER
        if ((i >= 0) && i < NumOfMenuItems) {
            GFX_UTIL::DrawRect(pScreen, _minX, _menuInfoBoxes[i].StartY, _maxX,
                               _menuInfoBoxes[i].EndY, g_color_red);
        }
#endif
    }
}* PMenuItemBoxes;

static MenuItemBoxes g_MenuItemBoxes = MenuItemBoxes();

const char* MenuItemEnumToString(MenuItemEnum e) {
    switch (e) {
        case MenuItemEnum::MENU_GAME:
            return "MENU_GAME";
        case MenuItemEnum::MENU_OPTIONS:
            return "MENU_OPTIONS";
        case MenuItemEnum::MENU_CREDITS:
            return "MENU_CREDITS";
        case MenuItemEnum::MENU_HELP:
            return "MENU_HELP";
        case MenuItemEnum::MENU_HIGHSCORE:
            return "MENU_HIGHSCORE";
        case MenuItemEnum::QUIT:
            return "QUIT";
        case MenuItemEnum::NOTHING:
            return "NOTHING";
        case MenuItemEnum::MENU_ROOT:
            return "MENU_ROOT";
        default:
            return "undef";
    }
}

////////////////////
MenuMgr::MenuMgr() {
    _p_fontDejBoldBig = 0;
    _p_fontDejSmall = 0;
    _p_fontDejUnderscoreSmall = 0;
    _p_ScreenBackbuffer = 0;
    _focusedMenuItem = MenuItemEnum::MENU_GAME;
    _p_MenuBox = 0;
    _p_SceneBackground = 0;
    _p_homeUrl = NULL;
    _p_LabelVersion = NULL;
}

MenuMgr::~MenuMgr() {
    if (_p_MenuBox) {
        SDL_DestroySurface(_p_MenuBox);
        _p_MenuBox = NULL;
    }
    if (_p_ScreenBackbuffer != NULL) {
        SDL_DestroySurface(_p_ScreenBackbuffer);
        _p_ScreenBackbuffer = NULL;
    }
    delete _p_homeUrl;
    delete _p_LabelVersion;
}

LPErrInApp MenuMgr::Initialize(SDL_Surface* pScreen,
                               UpdateScreenCb& fnUpdateScreen,
                               MenuDelegator& menuDelegator) {
    if (pScreen == NULL) {
        return ERR_UTIL::ErrorCreate("Screen is not initialized\n");
    }

    if (_p_ScreenBackbuffer != NULL) {
        SDL_DestroySurface(_p_ScreenBackbuffer);
        _p_ScreenBackbuffer = NULL;
    }
    delete _p_homeUrl;
    _p_homeUrl = NULL;
    delete _p_LabelVersion;
    _p_LabelVersion = NULL;
    _fnUpdateScreen = fnUpdateScreen;
    if (_fnUpdateScreen.tc == NULL) {
        return ERR_UTIL::ErrorCreate("MenuMgr: update screen is not defined\n");
    }
    _menuDlgt = menuDelegator;
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    _p_Screen = pScreen;

    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    _screenW = clipRect.w;
    _box_X = _screenW / 6;
    _screenH = clipRect.h;
    _box_Y = _screenH / 5;

    _rctPanelRedBox.w = 500;
    _rctPanelRedBox.h = 560;

    _rctPanelRedBox.x = (_screenW - _rctPanelRedBox.w) / 2;
    _rctPanelRedBox.y = (_screenH - _rctPanelRedBox.h) / 2;
    _box_X = _rctPanelRedBox.x;
    _box_Y = _rctPanelRedBox.y;
    TRACE("Menu Window dim: x: %d, y: %d - w: %d, h: %d\n", _box_X, _box_Y,
          _rctPanelRedBox.w, _rctPanelRedBox.h);
    g_MenuItemBoxes.SetBox(_rctPanelRedBox);

    _p_ScreenBackbuffer = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    _p_fontDejBoldBig = pGameSettings->GetFontDjvBoldBig();
    _p_fontDejSmall = pGameSettings->GetFontDjvSmall();

    _p_MenuBox = GFX_UTIL::SDL_CreateRGBSurface(
        _rctPanelRedBox.w, _rctPanelRedBox.h, 32, 0, 0, 0, 0);
    SDL_FillSurfaceRect(_p_MenuBox, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    SDL_SetSurfaceBlendMode(_p_MenuBox, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_MenuBox, 120);

    // link to invido.it
    _p_fontDejUnderscoreSmall = pGameSettings->GetFontDjvUnderscoreSmall();
    SDL_Rect rctBt1;
    rctBt1.h = 28;
    rctBt1.w = 150;
    rctBt1.y = _p_Screen->h - rctBt1.h - 20;
    rctBt1.x = _p_Screen->w - rctBt1.w - 20;
    _p_homeUrl = new LabelLinkGfx();
    ClickCb cbNUll = ClickCb{.tc = NULL, .self = NULL};
    _p_homeUrl->Initialize(&rctBt1, _p_ScreenBackbuffer,
                           _p_fontDejUnderscoreSmall, MYIDLABELURL, cbNUll);
    _p_homeUrl->SetState(LabelLinkGfx::INVISIBLE);
    _p_homeUrl->SetUrl(PACKAGE_URL);
    _p_homeUrl->SetWindowText(g_lpszMsgUrl);

    // label version
    _p_LabelVersion = new LabelGfx();
    rctBt1.h = 28;
    rctBt1.w = 150;
    rctBt1.y = _p_homeUrl->PosY() - 20;
    rctBt1.x = _p_homeUrl->PosX();
    _p_LabelVersion->Initialize(&rctBt1, _p_ScreenBackbuffer, _p_fontDejSmall);
    _p_LabelVersion->SetState(LabelGfx::INVISIBLE);
    _p_LabelVersion->SetWindowText(g_lpszVersion);

    SDL_GetSurfaceClipRect(_p_ScreenBackbuffer, &clipRect);
    SDL_FillSurfaceRect(
        _p_ScreenBackbuffer, &clipRect,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(_p_ScreenBackbuffer->format),
                    NULL, 60, 60, 60, 0));

    SDL_BlitSurface(_p_Screen, NULL, _p_ScreenBackbuffer, NULL);

    _p_GameSettings = GameSettings::GetSettings();
    _p_MusicManager = _p_GameSettings->GetMusicManager();
    _ignoreMouseEvent =
        pGameSettings->InputType == InputTypeEnum::TouchWithoutMouse;

    return NULL;
}

LPErrInApp MenuMgr::drawStaticScene() {
    LPLanguages pLanguages = _p_GameSettings->GetLanguageMan();
    SDL_Rect rctTarget = {0, 0, 0, 0};
    LPErrInApp err = NULL;
    rctTarget.x = (_p_ScreenBackbuffer->w - _p_SceneBackground->w) / 2;
    rctTarget.y = (_p_ScreenBackbuffer->h - _p_SceneBackground->h) / 2;
    rctTarget.w = _p_SceneBackground->w;
    rctTarget.h = _p_SceneBackground->h;

    // background img
    SDL_BlitSurface(_p_SceneBackground, NULL, _p_ScreenBackbuffer, &rctTarget);

    Uint32 colorBarTitle =
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_ScreenBackbuffer->format), NULL,
                   153, 202, 51);

    // content
    GFX_UTIL::DrawStaticSpriteEx(_p_ScreenBackbuffer, 0, 0, _rctPanelRedBox.w,
                                 _rctPanelRedBox.h, _rctPanelRedBox.x,
                                 _rctPanelRedBox.y, _p_MenuBox);

    // header bar
    int hbarOffset = 0;
    int hbar = 46;
    GFX_UTIL::FillRect(_p_ScreenBackbuffer, _box_X, _box_Y - (2 + hbarOffset),
                       _rctPanelRedBox.w, hbar, colorBarTitle);
    // wire
    GFX_UTIL::DrawRect(_p_ScreenBackbuffer, _box_X - 1, _box_Y - 1 - hbarOffset,
                       _rctPanelRedBox.w + _box_X + 1,
                       _rctPanelRedBox.h + _box_Y - hbarOffset, g_color_white);

    GFX_UTIL::DrawRect(_p_ScreenBackbuffer, _box_X - 2, _box_Y - 2 - hbarOffset,
                       _rctPanelRedBox.w + _box_X + 2,
                       _rctPanelRedBox.h + 2 + _box_Y - hbarOffset,
                       g_color_black);

    GFX_UTIL::DrawRect(_p_ScreenBackbuffer, _box_X, _box_Y,
                       _rctPanelRedBox.w + _box_X,
                       _rctPanelRedBox.h + _box_Y - hbarOffset, g_color_white);
    GFX_UTIL::DrawRect(_p_ScreenBackbuffer, _box_X, _box_Y - hbarOffset,
                       _rctPanelRedBox.w + _box_X,
                       _box_Y - hbarOffset + (hbarOffset - 2), g_color_white);
    // text title bar
    int bar_x = 30;
    int bar_y = 25;

    SDL_Color color = g_color_gray;
    err = drawMenuText(
        _p_ScreenBackbuffer,
        pLanguages->GetStringId(Languages::ID_WELCOMETITLEBAR).c_str(),
        _box_X + bar_x, _box_Y + bar_y - hbar / 2, color, _p_fontDejBoldBig);
    _hBar = hbar;
    return err;
}

LPErrInApp MenuMgr::drawMenuTextList() {
    SDL_Color color;
    LPErrInApp err;
    int offsetX = 30;
    int offsetY = 60;
    int morePlaceY = 60;
    int intraOffset = (_rctPanelRedBox.h - 80) / NumOfMenuItems;
    int minIntraOffsetY = 80;
    intraOffset = std::min(minIntraOffsetY, intraOffset);
    LPLanguages pLanguages = _p_GameSettings->GetLanguageMan();
    if (_rctPanelRedBox.h <= 600) {
        morePlaceY = 0;
    }
    // Play
    if (_focusedMenuItem != MenuItemEnum::MENU_GAME) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    int currY = _box_Y + offsetY + 30;
    err = drawMenuText(_p_ScreenBackbuffer,
                       pLanguages->GetStringId(Languages::ID_START).c_str(),
                       _box_X + offsetX, currY, color, _p_fontDejBoldBig);
    if (err != NULL) {
        return err;
    }
    int startY = _box_Y + _hBar;
    int endY = currY + morePlaceY + offsetY;

    g_MenuItemBoxes.drawMaxBox(_p_ScreenBackbuffer);
    g_MenuItemBoxes.SetPos0Y(startY, endY);
    g_MenuItemBoxes.drawBorder(0, _p_ScreenBackbuffer);

    // Options
    currY = currY + intraOffset + morePlaceY;
    if (_focusedMenuItem != MenuItemEnum::MENU_OPTIONS) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err =
        drawMenuText(_p_ScreenBackbuffer,
                     pLanguages->GetStringId(Languages::ID_MEN_OPTIONS).c_str(),
                     _box_X + offsetX, currY, color, _p_fontDejBoldBig);
    if (err != NULL) {
        return err;
    }
    endY = currY + morePlaceY + offsetY;
    g_MenuItemBoxes.SetYInPos(1, endY);
    g_MenuItemBoxes.drawBorder(1, _p_ScreenBackbuffer);

    // Credits
    currY += morePlaceY;
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_CREDITS) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       pLanguages->GetStringId(Languages::ID_CREDITS).c_str(),
                       _box_X + offsetX, currY, color, _p_fontDejBoldBig);
    if (err != NULL) {
        return err;
    }
    g_MenuItemBoxes.drawBorder(1, _p_ScreenBackbuffer);

    endY = currY + morePlaceY + offsetY;
    g_MenuItemBoxes.SetYInPos(2, endY);
    g_MenuItemBoxes.drawBorder(2, _p_ScreenBackbuffer);

    // Help
    currY += morePlaceY;
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_HELP) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       pLanguages->GetStringId(Languages::ID_MN_HELP).c_str(),
                       _box_X + offsetX, currY, color, _p_fontDejBoldBig);
    if (err != NULL) {
        return err;
    }

    endY = currY + morePlaceY + offsetY;
    g_MenuItemBoxes.SetYInPos(3, endY);
    g_MenuItemBoxes.drawBorder(3, _p_ScreenBackbuffer);

    // highscore
    currY += morePlaceY;
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_HIGHSCORE) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       pLanguages->GetStringId(Languages::ID_HIGHSCORE).c_str(),
                       _box_X + offsetX, currY, color, _p_fontDejBoldBig);
    if (err != NULL) {
        return err;
    }
    endY = currY + morePlaceY + offsetY;
    g_MenuItemBoxes.SetYInPos(4, endY);
    g_MenuItemBoxes.drawBorder(4, _p_ScreenBackbuffer);

#if HASQUITMENU
    // Quit
    currY += morePlaceY;
    currY = currY + intraOffset + 20;
    int lastY = _box_Y + _rctPanelRedBox.h - intraOffset - offsetY + 30;
    lastY = std::max(lastY, currY);
    if (_focusedMenuItem != MenuItemEnum::QUIT) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       pLanguages->GetStringId(Languages::ID_EXIT).c_str(),
                       _box_X + offsetX, lastY, color, _p_fontDejBoldBig);
    if (err != NULL) {
        return err;
    }
    g_MenuItemBoxes.SetYInPos(5, _box_Y + _rctPanelRedBox.h);
    g_MenuItemBoxes.drawBorder(5, _p_ScreenBackbuffer);
#endif
    return NULL;
}

LPErrInApp MenuMgr::HandleRootMenuEvent(SDL_Event* pEvent,
                                        const SDL_Point& targetPos) {
    LPErrInApp err;
    // TRACE_DEBUG("Ignore mouse events: %b", ignoreMouseEvent);
    if (pEvent->type == SDL_EVENT_QUIT) {
        (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
        return NULL;
    }
#if HASTOUCH
    SDL_Point touchLocation;
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        _p_GameSettings->GetTouchPoint(pEvent->tfinger, &touchLocation);
        TRACE_DEBUG("Tap in x=%d, y=%d\n", touchLocation.x, touchLocation.y);
        MenuItemBox tapInfoBox;
        if (g_MenuItemBoxes.IsPointInside(touchLocation, tapInfoBox)) {
            _focusedMenuItem = tapInfoBox.MenuItem;
            TRACE_DEBUG("Select menu %s from Tap down\n",
                        MenuItemEnumToString(_focusedMenuItem));
            err = rootMenuNext();
            if (err != NULL) {
                return err;
            }
        } else {
            TRACE_DEBUG("Tap outside the menu list\n");
            _focusedMenuItem = MenuItemEnum::NOTHING;
        }
    }
#endif

    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_UP) {
            _focusedMenuItem = previousMenu(_focusedMenuItem);
            if (_focusedMenuItem != MenuItemEnum::NOTHING) {
                _p_MusicManager->PlayEffect(MusicManager::EFFECT_OVER);
            }
        }
        if (pEvent->key.key == SDLK_DOWN) {
            _focusedMenuItem = nextMenu(_focusedMenuItem);
            if (_focusedMenuItem != MenuItemEnum::NOTHING) {
                _p_MusicManager->PlayEffect(MusicManager::EFFECT_OVER);
            }
        }
        if (pEvent->key.key == SDLK_RETURN) {
            TRACE_DEBUG("Select menu from return\n");
            err = rootMenuNext();
            if (err != NULL) {
                return err;
            }
        }
        if (pEvent->key.key == SDLK_ESCAPE) {
            (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
        }
    }
    if (pEvent->type == SDL_EVENT_MOUSE_MOTION) {
        if (_ignoreMouseEvent) {
            return NULL;
        }

        MenuItemBox mouseInfoBox;
        if (g_MenuItemBoxes.IsPointInside(targetPos, mouseInfoBox)) {
            _focusedMenuItem = mouseInfoBox.MenuItem;
            if (_focusedMenuItem != MenuItemEnum::NOTHING &&
                _focusedMenuItem != _prevFocusedMenuItem) {
                _p_MusicManager->PlayEffect(MusicManager::EFFECT_OVER);
                _prevFocusedMenuItem = _focusedMenuItem;
            }
        } else {
            _focusedMenuItem = MenuItemEnum::NOTHING;
        }
        _p_homeUrl->MouseMove(pEvent, targetPos);
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        MenuItemBox mouseInfoBox;

        if (g_MenuItemBoxes.IsPointInside(targetPos, mouseInfoBox)) {
            _focusedMenuItem = mouseInfoBox.MenuItem;
            TRACE_DEBUG("Select menu %s from mouse down\n",
                        MenuItemEnumToString(_focusedMenuItem));
            err = rootMenuNext();
            if (err != NULL) {
                return err;
            }
        } else {
            TRACE_DEBUG("Mouse outside the menu list\n");
            _focusedMenuItem = MenuItemEnum::NOTHING;
        }
    } else if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        _p_homeUrl->MouseUp(pEvent, targetPos);
    }

    return NULL;
}

LPErrInApp MenuMgr::HandleRootMenuIterate() {
    LPErrInApp err;
    // show the link url label
#if HASGOTLINK
    _p_homeUrl->SetState(LabelLinkGfx::VISIBLE);
#endif
    _p_LabelVersion->SetState(LabelGfx::VISIBLE);

    err = drawStaticScene();
    if (err != NULL) {
        return err;
    }
    err = drawMenuTextList();
    if (err != NULL) {
        return err;
    }
    _p_homeUrl->Draw(_p_ScreenBackbuffer);
    _p_LabelVersion->Draw(_p_ScreenBackbuffer);

    SDL_BlitSurface(_p_ScreenBackbuffer, NULL, _p_Screen, NULL);

    updateTextureAsFlipScreen();
    return NULL;
}

LPErrInApp MenuMgr::drawMenuText(SDL_Surface* psurf, const char* text, int x,
                                 int y, SDL_Color& color,
                                 TTF_Font* customfont) {
    LPErrInApp err = GFX_UTIL::DrawString(psurf, text, x + 2, y + 2,
                                          g_color_ombre, customfont);
    if (err != NULL)
        return err;
    return GFX_UTIL::DrawString(psurf, text, x, y, color, customfont);
}

void MenuMgr::updateTextureAsFlipScreen() {
    (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_Screen);
}

LPErrInApp MenuMgr::rootMenuNext() {
    LPErrInApp err;
    TRACE_DEBUG("Menu selected %s\n", MenuItemEnumToString(_focusedMenuItem));
    if (_focusedMenuItem == MenuItemEnum::QUIT) {
        err = (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
    } else {
        err = (_menuDlgt.tc)->EnterMenu(_menuDlgt.self, _focusedMenuItem);
    }
    return err;
}

MenuItemEnum previousMenu(MenuItemEnum currMenu) {
    switch (currMenu) {
        case MenuItemEnum::MENU_GAME:
            return MenuItemEnum::MENU_GAME;
        case MenuItemEnum::MENU_OPTIONS:
            return MenuItemEnum::MENU_GAME;
        case MenuItemEnum::MENU_CREDITS:
            return MenuItemEnum::MENU_OPTIONS;
        case MenuItemEnum::MENU_HELP:
            return MenuItemEnum::MENU_CREDITS;
        case MenuItemEnum::MENU_HIGHSCORE:
            return MenuItemEnum::MENU_HELP;
        case MenuItemEnum::QUIT:
            return MenuItemEnum::MENU_HIGHSCORE;
#if HASQUITMENU
        case MenuItemEnum::NOTHING:
            return MenuItemEnum::QUIT;
#else
        case MenuItemEnum::NOTHING:
            return MenuItemEnum::MENU_HIGHSCORE;
#endif
        default:
            return currMenu;
    }
    return currMenu;
}

MenuItemEnum nextMenu(MenuItemEnum currMenu) {
    switch (currMenu) {
        case MenuItemEnum::MENU_GAME:
            return MenuItemEnum::MENU_OPTIONS;
        case MenuItemEnum::MENU_OPTIONS:
            return MenuItemEnum::MENU_CREDITS;
        case MenuItemEnum::MENU_CREDITS:
            return MenuItemEnum::MENU_HELP;
        case MenuItemEnum::MENU_HELP:
            return MenuItemEnum::MENU_HIGHSCORE;
#if HASQUITMENU
        case MenuItemEnum::MENU_HIGHSCORE:
            return MenuItemEnum::QUIT;
#else
        case MenuItemEnum::MENU_HIGHSCORE:
            return MenuItemEnum::NOTHING;
#endif
        case MenuItemEnum::QUIT:
            return MenuItemEnum::QUIT;
        case MenuItemEnum::NOTHING:
            return MenuItemEnum::MENU_GAME;
        default:
            return currMenu;
    }
    return currMenu;
}

void MenuMgr::UpdateScreen(SDL_Surface* pScreen) {
    if (pScreen == NULL)
        return;
    TRACE("MenuMgr::UpdateScreen\n");
    _p_Screen = pScreen;

    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    _screenW = clipRect.w;
    _screenH = clipRect.h;

    _rctPanelRedBox.x = (_screenW - _rctPanelRedBox.w) / 2;
    _rctPanelRedBox.y = (_screenH - _rctPanelRedBox.h) / 2;
    _box_X = _rctPanelRedBox.x;
    _box_Y = _rctPanelRedBox.y;
    g_MenuItemBoxes.SetBox(_rctPanelRedBox);

    if (_p_ScreenBackbuffer != NULL) {
        SDL_DestroySurface(_p_ScreenBackbuffer);
    }
    _p_ScreenBackbuffer = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    // Re-initialize labels because they depend on screen size/backbuffer
    SDL_Rect rctBt1;
    rctBt1.h = 28;
    rctBt1.w = 150;
    rctBt1.y = _p_Screen->h - rctBt1.h - 20;
    rctBt1.x = _p_Screen->w - rctBt1.w - 20;

    ClickCb cbNUll = ClickCb{.tc = NULL, .self = NULL};
    _p_homeUrl->Initialize(&rctBt1, _p_ScreenBackbuffer,
                           _p_fontDejUnderscoreSmall, MYIDLABELURL, cbNUll);

    rctBt1.y = _p_homeUrl->PosY() - 20;
    rctBt1.x = _p_homeUrl->PosX();
    _p_LabelVersion->Initialize(&rctBt1, _p_ScreenBackbuffer, _p_fontDejSmall);
}
