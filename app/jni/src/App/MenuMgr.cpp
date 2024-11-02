#include "MenuMgr.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "CompGfx/LabelGfx.h"
#include "CompGfx/LabelLinkGfx.h"
#include "Config.h"
#include "ErrorInfo.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "WinTypeGlobal.h"

static const char* g_lpszMsgUrl = "Go to invido.it";
static const char* g_lpszVersion = "Ver 2.3.1 20231128";
static const char* g_lpszIniFontVera = DATA_PREFIX "font/vera.ttf";

static const SDL_Color g_color_on = {253, 252, 250};
static const SDL_Color g_color_off = {128, 128, 128};
static const SDL_Color g_color_white = {255, 255, 255};
static const SDL_Color g_color_ombre = {87, 87, 87, 50};
static const SDL_Color g_color_black = {0, 0, 0};
static const SDL_Color g_color_gray = {128, 128, 128};
static const SDL_Color g_color_red = {255, 0, 0};

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

static const int NumOfMenuItems = 6;
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
        _menuInfoBoxes[5] = {0, 0, MenuItemEnum::QUIT};
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
        tapped.MenuItem = _menuInfoBoxes[4].MenuItem;
        return true;
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
}* PMenuItemBoxes;

static MenuItemBoxes g_MenuItemBoxes = MenuItemBoxes();

constexpr const char* MenuItemEnumToString(MenuItemEnum e) {
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
        default:
            return "undef";
    }
}

////////////////////
MenuMgr::MenuMgr() {
    _p_fontAriblk = 0;
    _p_fontVera = 0;
    _p_fontVeraUnderscore = 0;
    _p_ScreenBackbuffer = 0;
    _focusedMenuItem = MenuItemEnum::MENU_GAME;
    _p_Languages = 0;
    _p_MenuBox = 0;
    _p_SceneBackground = 0;
    _p_homeUrl = NULL;
    _p_LabelVersion = NULL;
}

MenuMgr::~MenuMgr() {
    if (_p_MenuBox) {
        SDL_FreeSurface(_p_MenuBox);
        _p_MenuBox = NULL;
    }
    if (_p_ScreenBackbuffer != NULL) {
        SDL_FreeSurface(_p_ScreenBackbuffer);
        _p_ScreenBackbuffer = NULL;
    }
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
        _p_ScreenTexture = NULL;
    }
    delete _p_homeUrl;
    delete _p_LabelVersion;
}

LPErrInApp MenuMgr::Initialize(SDL_Surface* pScreen, SDL_Renderer* pRenderer,
                               SDL_Window* pWindow,
                               MenuDelegator& menuDelegator) {
    _menuDlgt = menuDelegator;
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    _p_Screen = pScreen;
    _p_sdlRenderer = pRenderer;
    _p_Window = pWindow;
    int sizeBigFactorH = 1;
    int sizeBigFactorW = 1;
    if (pGameSettings->NeedScreenMagnify()) {
        sizeBigFactorH = 2;
    }
    if (_p_Screen->w > 1200) {
        sizeBigFactorW = 2;
    }

    _screenW = _p_Screen->clip_rect.w;
    _box_X = _screenW / 6;
    _screenH = _p_Screen->clip_rect.h;
    _box_Y = _screenH / 5;

    _rctPanelRedBox.w = _screenW - _box_X * 2;
    _rctPanelRedBox.h = _screenH - _box_Y * 2;
    _rctPanelRedBox.w = max(_rctPanelRedBox.w, 800);
    _rctPanelRedBox.h = max(_rctPanelRedBox.h, 600);
    if (_rctPanelRedBox.w > 1024) {
        _rctPanelRedBox.w = 1024;
    }
    if (_rctPanelRedBox.h > 1200) {
        _rctPanelRedBox.h = 1200;
    }

    _rctPanelRedBox.x = (_screenW - _rctPanelRedBox.w) / 2;
    _rctPanelRedBox.y = (_screenH - _rctPanelRedBox.h) / 2;
    _box_X = _rctPanelRedBox.x;
    _box_Y = _rctPanelRedBox.y;
    TRACE("Menu Window dim: x: %d, y: %d - w: %d, h: %d\n", _box_X, _box_Y,
          _rctPanelRedBox.w, _rctPanelRedBox.h);
    g_MenuItemBoxes.SetBox(_rctPanelRedBox);

    _p_ScreenBackbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, _p_Screen->w,
                                               _p_Screen->h, 32, 0, 0, 0, 0);
    _p_ScreenTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         _p_Screen->w, _p_Screen->h);
    if (_p_ScreenTexture == NULL) {
        return ERR_UTIL::ErrorCreate("MenuMgr: Cannot create texture: %s\n",
                                     SDL_GetError());
    }

    _p_fontAriblk = (_menuDlgt.tc)->GetFontAriblk(_menuDlgt.self);
    _p_fontVera = (_menuDlgt.tc)->GetFontVera(_menuDlgt.self);
    _p_Languages = (_menuDlgt.tc)->GetLanguageMan(_menuDlgt.self);

    _p_MenuBox = SDL_CreateRGBSurface(SDL_SWSURFACE, _rctPanelRedBox.w,
                                      _rctPanelRedBox.h, 32, 0, 0, 0, 0);
    SDL_FillRect(_p_MenuBox, NULL,
                 SDL_MapRGBA(_p_Screen->format, 136, 60, 60, 0));
    SDL_SetSurfaceBlendMode(_p_MenuBox, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_MenuBox, 127);

    // link to invido.it
    _p_fontVeraUnderscore =
        TTF_OpenFont(g_lpszIniFontVera, pGameSettings->GetSizeFontSmall());
    if (_p_fontVeraUnderscore == 0) {
        return ERR_UTIL::ErrorCreate(
            "MenuMgr: Unable to load font %s, error: %s\n", g_lpszIniFontVera,
            SDL_GetError());
    }
    TTF_SetFontStyle(_p_fontVeraUnderscore, TTF_STYLE_UNDERLINE);
    SDL_Rect rctBt1;
    if (pGameSettings->NeedScreenMagnify()) {
        rctBt1.h = 56;
        rctBt1.w = 200;
        rctBt1.y = _p_Screen->h - rctBt1.h - 50;
        rctBt1.x = _p_Screen->w - rctBt1.w - 90;
    } else {
        rctBt1.h = 28;
        rctBt1.w = 150;
        rctBt1.y = _p_Screen->h - rctBt1.h - 20;
        rctBt1.x = _p_Screen->w - rctBt1.w - 20;
    }
    _p_homeUrl = new LabelLinkGfx();
    ClickCb cbNUll = ClickCb{.tc = NULL, .self = NULL};
    _p_homeUrl->Initialize(&rctBt1, _p_ScreenBackbuffer, _p_fontVeraUnderscore,
                           MYIDLABELURL, cbNUll);
    _p_homeUrl->SetState(LabelLinkGfx::INVISIBLE);
    _p_homeUrl->SetUrl(PACKAGE_URL);
    _p_homeUrl->SetWindowText(g_lpszMsgUrl);

    // label version
    _p_LabelVersion = new LabelGfx();
    if (pGameSettings->NeedScreenMagnify()) {
        rctBt1.h = 56;
        rctBt1.w = 200;
        rctBt1.y = _p_homeUrl->PosY() - 60;
        rctBt1.x = _p_homeUrl->PosX();
    } else {
        rctBt1.h = 28;
        rctBt1.w = 150;
        rctBt1.y = _p_homeUrl->PosY() - 20;
        rctBt1.x = _p_homeUrl->PosX();
    }
    _p_LabelVersion->Initialize(&rctBt1, _p_ScreenBackbuffer, _p_fontVera);
    _p_LabelVersion->SetState(LabelGfx::INVISIBLE);
    _p_LabelVersion->SetWindowText(g_lpszVersion);

    SDL_FillRect(_p_ScreenBackbuffer, &_p_ScreenBackbuffer->clip_rect,
                 SDL_MapRGBA(_p_ScreenBackbuffer->format, 60, 60, 60, 0));
    SDL_BlitSurface(_p_Screen, NULL, _p_ScreenBackbuffer, NULL);

    return NULL;
}

LPErrInApp MenuMgr::drawStaticScene() {
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    // function called in loop
    SDL_Rect rctTarget;
    LPErrInApp err = NULL;
    rctTarget.x = (_p_ScreenBackbuffer->w - _p_SceneBackground->w) / 2;
    rctTarget.y = (_p_ScreenBackbuffer->h - _p_SceneBackground->h) / 2;
    rctTarget.w = _p_SceneBackground->w;
    rctTarget.h = _p_SceneBackground->h;

    // background img
    SDL_BlitSurface(_p_SceneBackground, NULL, _p_ScreenBackbuffer, &rctTarget);

    Uint32 colorBarTitle =
        SDL_MapRGB(_p_ScreenBackbuffer->format, 153, 202, 51);

    // content
    GFX_UTIL::DrawStaticSpriteEx(_p_ScreenBackbuffer, 0, 0, _rctPanelRedBox.w,
                                 _rctPanelRedBox.h, _rctPanelRedBox.x,
                                 _rctPanelRedBox.y, _p_MenuBox);

    // header bar
    int hbarOffset = 0;
    int hbar = 46;
    if (pGameSettings->NeedScreenMagnify()) {
        hbar = 65;
    }
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
        _p_Languages->GetStringId(Languages::ID_WELCOMETITLEBAR).c_str(),
        _box_X + bar_x, _box_Y + bar_y - hbar / 2, color, _p_fontAriblk);
    return err;
}

LPErrInApp MenuMgr::drawMenuTextList() {
    SDL_Color color;
    LPErrInApp err;
    int offsetX = 30;
    int offsetY = 60;
    int morePlaceY = 60;
    int intraOffset = (_rctPanelRedBox.h - 80) / 5;
    int minIntraOffsetY = 80;
    intraOffset = min(minIntraOffsetY, intraOffset);
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    if (pGameSettings->NeedScreenMagnify()) {
        intraOffset += 50;
    }
    // Play
    if (_focusedMenuItem != MenuItemEnum::MENU_GAME) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    int currY = _box_Y + offsetY + 30;
    err = drawMenuText(_p_ScreenBackbuffer,
                       _p_Languages->GetStringId(Languages::ID_START).c_str(),
                       _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    currY += morePlaceY;
#ifdef ANDROID
    currY += 2 * morePlaceY;
#endif
    g_MenuItemBoxes.SetPos0Y(_box_Y, currY);
    // Options
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_OPTIONS) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(
        _p_ScreenBackbuffer,
        _p_Languages->GetStringId(Languages::ID_MEN_OPTIONS).c_str(),
        _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    currY += morePlaceY;
    g_MenuItemBoxes.SetYInPos(1, currY);
    // Credits
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_CREDITS) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       _p_Languages->GetStringId(Languages::ID_CREDITS).c_str(),
                       _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    currY += morePlaceY;
    g_MenuItemBoxes.SetYInPos(2, currY);

    // Help
#ifndef ANDROID
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_HELP) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       _p_Languages->GetStringId(Languages::ID_MN_HELP).c_str(),
                       _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    currY += morePlaceY;
    g_MenuItemBoxes.SetYInPos(3, currY);
#endif

    // highscore
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_HIGHSCORE) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err =
        drawMenuText(_p_ScreenBackbuffer,
                     _p_Languages->GetStringId(Languages::ID_HIGHSCORE).c_str(),
                     _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    currY += morePlaceY;
    g_MenuItemBoxes.SetYInPos(4, currY);

#ifndef ANDROID
    // Quit
    currY = currY + intraOffset + 20;
    int lastY = _box_Y + _rctPanelRedBox.h - intraOffset - offsetY + 30;
    lastY = max(lastY, currY);
    if (_focusedMenuItem != MenuItemEnum::QUIT) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       _p_Languages->GetStringId(Languages::ID_EXIT).c_str(),
                       _box_X + offsetX, lastY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    g_MenuItemBoxes.SetYInPos(
        4, lastY - 1);  // fill the space until quit with menu 4, not the 5
    g_MenuItemBoxes.SetYInPos(5, _box_Y + _rctPanelRedBox.h);
#endif
    return NULL;
}

LPErrInApp MenuMgr::HandleRootMenu() {
    LPErrInApp err;
    // show the link url label
    _p_homeUrl->SetState(LabelLinkGfx::VISIBLE);
    _p_LabelVersion->SetState(LabelGfx::VISIBLE);

    SDL_Color color = g_color_white;
    err = drawStaticScene();
    if (err != NULL) {
        return err;
    }
    err = drawMenuTextList();
    if (err != NULL) {
        return err;
    }

    SDL_Point touchLocation;
    bool mouseInside;
    SDL_Event event;
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    bool ignoreMouseEvent =
        pGameSettings->InputType == InputTypeEnum::TouchWithoutMouse;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
            break;
        }

        if (event.type == SDL_FINGERDOWN) {
            pGameSettings->GetTouchPoint(event.tfinger, &touchLocation);
            TRACE_DEBUG("Tap in x=%d, y=%d\n", touchLocation.x,
                        touchLocation.y);
            MenuItemBox tapInfoBox;
            if (g_MenuItemBoxes.IsPointInside(touchLocation, tapInfoBox)) {
                _focusedMenuItem = tapInfoBox.MenuItem;
                TRACE_DEBUG("Select menu %s from Tap down\n",
                            MenuItemEnumToString(_focusedMenuItem));
                rootMenuNext();
            } else {
                TRACE_DEBUG("Tap outside the menu list\n");
            }
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_UP) {
                _focusedMenuItem = previousMenu(_focusedMenuItem);
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                _focusedMenuItem = nextMenu(_focusedMenuItem);
            }
            if (event.key.keysym.sym == SDLK_RETURN) {
                TRACE_DEBUG("Select menu from return\n");
                rootMenuNext();
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
            }
        }
        if (event.type == SDL_MOUSEMOTION) {
            if (ignoreMouseEvent) {
                break;
            }
            SDL_Point motionLocation = {event.motion.x, event.motion.y};
            MenuItemBox mouseInfoBox;
            if (g_MenuItemBoxes.IsPointInside(motionLocation, mouseInfoBox)) {
                _focusedMenuItem = mouseInfoBox.MenuItem;
                mouseInside = true;
            } else {
                mouseInside = false;
            }
            _p_homeUrl->MouseMove(event);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (ignoreMouseEvent) {
                break;
            }
            if (mouseInside) {
                TRACE_DEBUG("Select menu from mouse down\n");
                rootMenuNext();
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (ignoreMouseEvent) {
                break;
            }
            _p_homeUrl->MouseUp(event);
        }
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
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    SDL_RenderCopy(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);
}

void MenuMgr::rootMenuNext() {
    TRACE_DEBUG("Menu selected %s\n", MenuItemEnumToString(_focusedMenuItem));
    if (_focusedMenuItem == MenuItemEnum::QUIT) {
        (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
    } else {
        (_menuDlgt.tc)->SetNextMenu(_menuDlgt.self, _focusedMenuItem);
    }
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
        case MenuItemEnum::MENU_HIGHSCORE:
            return MenuItemEnum::QUIT;
        case MenuItemEnum::QUIT:
            return MenuItemEnum::QUIT;
        default:
            return currMenu;
    }
    return currMenu;
}
