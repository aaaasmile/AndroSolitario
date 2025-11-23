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
#include "WinTypeGlobal.h"

static const char* g_lpszMsgUrl = "Go to invido.it";
static const char* g_lpszVersion = VERSION;
static const char* g_lpszIniFontVera = DATA_PREFIX "font/vera.ttf";

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
    _p_fontAriblk = 0;
    _p_fontVera = 0;
    _p_fontVeraUnderscore = 0;
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
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    _p_Screen = pScreen;
    _p_sdlRenderer = pRenderer;
    _p_Window = pWindow;

    SDL_Rect clipRect; 
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    _screenW = clipRect.w;
    _box_X = _screenW / 6;
    _screenH = clipRect.h;
    _box_Y = _screenH / 5;

    if (_screenW == 1024 && _screenH == 768) {
        _rctPanelRedBox.w = 500;
        _rctPanelRedBox.h = 560;
    } else {
        _rctPanelRedBox.w = _screenW - _box_X * 2;
        _rctPanelRedBox.h = _screenH - _box_Y * 2;
        _rctPanelRedBox.w = std::max(_rctPanelRedBox.w, 800);
        _rctPanelRedBox.h = std::max(_rctPanelRedBox.h, 600);
        if (_rctPanelRedBox.w > 1024) {
            _rctPanelRedBox.w = 1024;
        }
        if (_rctPanelRedBox.h > 1200) {
            _rctPanelRedBox.h = 1200;
        }
    }

    _rctPanelRedBox.x = (_screenW - _rctPanelRedBox.w) / 2;
    _rctPanelRedBox.y = (_screenH - _rctPanelRedBox.h) / 2;
    _box_X = _rctPanelRedBox.x;
    _box_Y = _rctPanelRedBox.y;
    TRACE("Menu Window dim: x: %d, y: %d - w: %d, h: %d\n", _box_X, _box_Y,
          _rctPanelRedBox.w, _rctPanelRedBox.h);
    g_MenuItemBoxes.SetBox(_rctPanelRedBox);

    _p_ScreenBackbuffer = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    _p_ScreenTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         _p_Screen->w, _p_Screen->h);
    if (_p_ScreenTexture == NULL) {
        return ERR_UTIL::ErrorCreate("MenuMgr: Cannot create texture: %s\n",
                                     SDL_GetError());
    }

    _p_fontAriblk = pGameSettings->GetFontAriblk();
    _p_fontVera = pGameSettings->GetFontVera();

    _p_MenuBox = GFX_UTIL::SDL_CreateRGBSurface(
        _rctPanelRedBox.w, _rctPanelRedBox.h, 32, 0, 0, 0, 0);
    SDL_FillSurfaceRect(_p_MenuBox, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    SDL_SetSurfaceBlendMode(_p_MenuBox, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_MenuBox, 120);

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
    SDL_Rect rctTarget;
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
    if (_p_GameSettings->NeedScreenMagnify()) {
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
        pLanguages->GetStringId(Languages::ID_WELCOMETITLEBAR).c_str(),
        _box_X + bar_x, _box_Y + bar_y - hbar / 2, color, _p_fontAriblk);
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
    if (_p_GameSettings->NeedScreenMagnify()) {
        intraOffset += 50;
    }
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
                       _box_X + offsetX, currY, color, _p_fontAriblk);
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
                     _box_X + offsetX, currY, color, _p_fontAriblk);
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
                       _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    g_MenuItemBoxes.drawBorder(1, _p_ScreenBackbuffer);

    endY = currY + morePlaceY + offsetY;
    g_MenuItemBoxes.SetYInPos(2, endY);
    g_MenuItemBoxes.drawBorder(2, _p_ScreenBackbuffer);

    // Help
#if HASHELPMENU
    currY += morePlaceY;
    currY = currY + intraOffset;
    if (_focusedMenuItem != MenuItemEnum::MENU_HELP) {
        color = g_color_off;
    } else {
        color = g_color_on;
    }
    err = drawMenuText(_p_ScreenBackbuffer,
                       pLanguages->GetStringId(Languages::ID_MN_HELP).c_str(),
                       _box_X + offsetX, currY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    endY = currY + morePlaceY + offsetY;
    g_MenuItemBoxes.SetYInPos(3, endY);
    g_MenuItemBoxes.drawBorder(3, _p_ScreenBackbuffer);
#endif

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
                       _box_X + offsetX, currY, color, _p_fontAriblk);
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
                       _box_X + offsetX, lastY, color, _p_fontAriblk);
    if (err != NULL) {
        return err;
    }
    g_MenuItemBoxes.SetYInPos(5, _box_Y + _rctPanelRedBox.h);
    g_MenuItemBoxes.drawBorder(5, _p_ScreenBackbuffer);
#endif
    return NULL;
}

LPErrInApp MenuMgr::HandleRootMenuEvent(SDL_Event* pEvent) {
    SDL_Point touchLocation;
    // TRACE_DEBUG("Ignore mouse events: %b", ignoreMouseEvent);
    if (pEvent->type == SDL_EVENT_QUIT) {
        (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
        return NULL;
    }

    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        _p_GameSettings->GetTouchPoint(pEvent->tfinger, &touchLocation);
        TRACE_DEBUG("Tap in x=%d, y=%d\n", touchLocation.x, touchLocation.y);
        MenuItemBox tapInfoBox;
        if (g_MenuItemBoxes.IsPointInside(touchLocation, tapInfoBox)) {
            _focusedMenuItem = tapInfoBox.MenuItem;
            TRACE_DEBUG("Select menu %s from Tap down\n",
                        MenuItemEnumToString(_focusedMenuItem));
            rootMenuNext();
        } else {
            TRACE_DEBUG("Tap outside the menu list\n");
            _focusedMenuItem = MenuItemEnum::NOTHING;
        }
    }

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
            rootMenuNext();
        }
        if (pEvent->key.key == SDLK_ESCAPE) {
            (_menuDlgt.tc)->LeaveMenu(_menuDlgt.self);
        }
    }
    if (pEvent->type == SDL_EVENT_MOUSE_MOTION) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        SDL_Point motionLocation;
        motionLocation.x = (int)pEvent->motion.x;
        motionLocation.y = (int)pEvent->motion.y;

        MenuItemBox mouseInfoBox;
        if (g_MenuItemBoxes.IsPointInside(motionLocation, mouseInfoBox)) {
            _focusedMenuItem = mouseInfoBox.MenuItem;
            if (_focusedMenuItem != MenuItemEnum::NOTHING &&
                _focusedMenuItem != _prevFocusedMenuItem) {
                _p_MusicManager->PlayEffect(MusicManager::EFFECT_OVER);
                _prevFocusedMenuItem = _focusedMenuItem;
            }
        } else {
            _focusedMenuItem = MenuItemEnum::NOTHING;
        }
        _p_homeUrl->MouseMove(*pEvent);
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        MenuItemBox mouseInfoBox;
        SDL_Point clickLocation;
        clickLocation.x = (int)pEvent->button.x;
        clickLocation.y = (int)pEvent->button.y;

        if (g_MenuItemBoxes.IsPointInside(clickLocation, mouseInfoBox)) {
            _focusedMenuItem = mouseInfoBox.MenuItem;
            TRACE_DEBUG("Select menu %s from mouse down\n",
                        MenuItemEnumToString(_focusedMenuItem));
            rootMenuNext();
        } else {
            TRACE_DEBUG("Mouse outside the menu list\n");
            _focusedMenuItem = MenuItemEnum::NOTHING;
        }
    } else if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        _p_homeUrl->MouseUp(*pEvent);
    }

    return NULL;
}

LPErrInApp MenuMgr::HandleRootMenuIterate() {
    LPErrInApp err;
    // show the link url label
    _p_homeUrl->SetState(LabelLinkGfx::VISIBLE);
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
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
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
#if HASHELPMENU
        case MenuItemEnum::MENU_HIGHSCORE:
            return MenuItemEnum::MENU_HELP;
#else
           case MenuItemEnum::MENU_HIGHSCORE:
            return MenuItemEnum::MENU_CREDITS;
#endif
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
#if HASHELPMENU
        case MenuItemEnum::MENU_CREDITS:
            return MenuItemEnum::MENU_HELP;
#else
        case MenuItemEnum::MENU_CREDITS:
            return MenuItemEnum::MENU_HIGHSCORE;
#endif
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
