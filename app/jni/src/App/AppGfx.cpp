#include "AppGfx.h"

#include <SDL3_image/SDL_image.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include "Config.h"
#include "Credits.h"
#include "ErrorInfo.h"
#include "Fading.h"
#include "GfxUtil.h"
#include "HighScore.h"
#include "MenuMgr.h"
#include "MusicManager.h"
#include "OptionsGfx.h"
#include "WinTypeGlobal.h"

static const char* g_lpszHelpFileName = DATA_PREFIX "solitario.pdf";

static const char* g_lpszIconProgFile = DATA_PREFIX "images/icona_asso.bmp";
static const char* g_lpszTitleFile = DATA_PREFIX "images/title.png";

static const char* g_lpszImageSplashComm =
    DATA_PREFIX "images/commessaggio.jpg";
static const char* g_lpszImageSplashMantova = DATA_PREFIX "images/mantova.jpg";

typedef struct ResolutionMgr {
    int targetWidth, targetHeight;
    int displayWidth, displayHeight;
    float scale;
    bool showFullPortrait;
    bool isDev;
    SDL_FRect viewport = {0.0, 0.0, 0.0, 0.0};
}* LPResolutionMgr;

static void resolutionMgr_UpdateViewport(ResolutionMgr& rm, int w, int h) {
    rm.displayWidth = w;
    rm.displayHeight = h;

    TRACE_DEBUG("[UpdateViewport] Resize win, new width %d, height %d \n", w,
                h);

    float scaledWidth = rm.targetWidth * rm.scale;
    float scaledHeight = rm.targetHeight * rm.scale;

    rm.viewport.x = (rm.displayWidth - scaledWidth) * 0.5f;    // Centered X
    rm.viewport.y = (rm.displayHeight - scaledHeight) * 0.5f;  // Centered Y
    rm.viewport.w = scaledWidth;
    rm.viewport.h = scaledHeight;
}

static void resolutionMgr_InitPortraitDev(ResolutionMgr& rm) {
    TRACE("Portrait DEV mode \n");
    rm.targetWidth = 720;
    rm.targetHeight = 1280;
    rm.showFullPortrait = false;
    rm.isDev = true;
    rm.scale = 0.7;
    rm.displayWidth = (int)(rm.targetWidth * rm.scale);
    rm.displayHeight = (int)(rm.targetHeight * rm.scale);
    resolutionMgr_UpdateViewport(rm, rm.displayWidth, rm.displayHeight);
}

static void resolutionMgr_InitStandard(ResolutionMgr& rm) {
    TRACE("Standard mode \n");
    rm.targetWidth = 1024;
    rm.targetHeight = 768;
    rm.showFullPortrait = false;
    rm.scale = 1.0;
    rm.isDev = false;
    rm.displayWidth = rm.targetWidth;
    rm.displayHeight = rm.targetHeight;
}

static void resolutionMgr_InitFullPortrait(ResolutionMgr& rm) {
    TRACE("Full portrait mode \n");
    rm.targetWidth = 720;
    rm.targetHeight = 1280;
    rm.showFullPortrait = true;
    rm.scale = 1.0;
    rm.isDev = false;
    rm.displayWidth = rm.targetWidth * rm.scale;
    rm.displayHeight = rm.targetHeight * rm.scale;

    int num_displays;
    SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);
    if (num_displays == 0) {
        return;
    }
    SDL_DisplayID Id = *displays;
    SDL_Rect screenRect;
    if (!SDL_GetDisplayBounds(Id, &screenRect)) {
        TRACE("CalcDisplaySize error: %s\n", SDL_GetError());
        return;
    }
    if (screenRect.w > rm.displayWidth) {
        rm.displayWidth = screenRect.w;
    }
    if (screenRect.h > rm.displayHeight) {
        rm.displayHeight = screenRect.h;
    }
    TRACE_DEBUG("Display bound for size is width %d, height %d",
                rm.displayWidth, rm.displayHeight);

    SDL_free(displays);
}

static ResolutionMgr g_ResolutionMgr;

AppGfx::AppGfx() {
    _p_Window = NULL;
    _p_ScreenTexture = NULL;
    _p_SolitarioGfx = NULL;
    _p_SceneBackground = NULL;
    _p_Screen = NULL;
    _lastMainLoopticks = 0;
    _Bpp = 0;
    _p_MusicManager = NULL;
    _p_CreditTitle = NULL;
    _fullScreen = false;
    _p_GameSettings = GameSettings::GetSettings();
    _p_CreditsView = new CreditsView();
    _p_OptGfx = new OptionsGfx();
    _p_HighScore = new HighScore();
    // #ifdef ANDROID
    //     // this is portrait
    //     //_screenW = 1080;
    //     //_screenH = 1920;
    //     resolutionMgr_InitFullPortrait(g_ResolutionMgr);
    // #else

    //     //_screenH = 768;   // 960; //768;
    //     //_screenW = 1024;  // 540;//1024;
    // #endif
}

AppGfx::~AppGfx() { terminate(); }

LPErrInApp AppGfx::Init() {
    TRACE("Init App\n");
    //_p_GameSettings->GameName = "Solitario";
#ifdef WIN32
    LPCSTR exeDirPath = GAMESET::GetExeAppFolder();
    TRACE("Exe directory is %s\n", exeDirPath);
    if (chdir(exeDirPath) < 0) {
        return ERR_UTIL::ErrorCreate("Unable to change to the exe directory");
    } else {
        TRACE("Dir changed to %s\n", exeDirPath);
    }
#endif
#ifdef ANDROID
    _p_GameSettings->InputType = InputTypeEnum::TouchWithoutMouse;
#endif

    LPErrInApp err = loadProfile();
    if (err != NULL) {
        return err;
    }

    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (!SDL_Init(0)) {
            return ERR_UTIL::ErrorCreate("Couldn't initialize SDL: %s\n",
                                         SDL_GetError());
        }
    }
    if (_p_GameSettings->IsPortrait()) {
        if (_p_GameSettings->IsFullPortrait()) {
            resolutionMgr_InitFullPortrait(g_ResolutionMgr);
        } else {
            resolutionMgr_InitPortraitDev(g_ResolutionMgr);
        }
    } else {
        resolutionMgr_InitStandard(g_ResolutionMgr);
    }

    err = createWindow();
    if (err != NULL) {
        return err;
    }
    _p_HighScore->Load();

    _p_GameSettings->SetCurrentLang();
    Languages* pLanguages = _p_GameSettings->GetLanguageMan();

    if (!TTF_Init()) {
        return ERR_UTIL::ErrorCreate("Font init error");
    }
    // if (_p_GameSettings->NeedScreenMagnify()) {
    //     _p_GameSettings->UseBigFontSize();
    // }
    TRACE_DEBUG("Font initialized OK\n");

    const char* title = pLanguages->GetCStringId(Languages::ID_SOLITARIO);
    SDL_SetWindowTitle(_p_Window, title);

    SDL_Surface* psIcon = SDL_LoadBMP(g_lpszIconProgFile);
    if (psIcon == 0) {
        return ERR_UTIL::ErrorCreate("Icon not found");
    }
    TRACE_DEBUG("Icon loaded OK\n");
    Uint8 r, g, b, a;
    SDL_ReadSurfacePixel(psIcon, 0, 0, &r, &g, &b, &a);
    SDL_SetSurfaceColorKey(
        psIcon, true,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(psIcon->format), NULL, r, g, b,
                    a));
#if HASWINICON
    if (!SDL_SetWindowIcon(_p_Window, psIcon)) {
        return ERR_UTIL::ErrorCreate("Couldn't set icon on window: %s\n",
                                     SDL_GetError());
    }
#endif
    _p_CreditTitle = IMG_Load(g_lpszTitleFile);
    if (_p_CreditTitle == 0) {
        return ERR_UTIL::ErrorCreate("Title image not found");
    }
    TRACE_DEBUG("Credit loaded OK\n");

    err = loadSceneBackground();
    if (err != NULL) {
        return err;
    }
    TRACE_DEBUG("SceneBackground OK\n");
    err = _p_GameSettings->LoadFonts();
    if (err != NULL) {
        TRACE_DEBUG("Error on Load fonts\n");
        return err;
    }
    TRACE_DEBUG("Fonts and background loaded OK\n");

    clearBackground();

    err = _p_GameSettings->InitMusicManager();
    if (err != NULL) {
        return err;
    }
    _p_MusicManager = _p_GameSettings->GetMusicManager();
    err = _p_MusicManager->LoadMusicRes();
    if (err != NULL) {
        return err;
    }

    MenuDelegator menuDelegator = prepMenuDelegator();
    UpdateScreenCb screenUpdater = prepScreenUpdater();
    _p_MenuMgr = new MenuMgr();
    // err = _p_MenuMgr->Initialize(_p_Screen, _p_sdlRenderer, _p_Window,
    //                              menuDelegator);
    err = _p_MenuMgr->Initialize(_p_Screen, screenUpdater, menuDelegator);
    if (err != NULL) {
        return err;
    }
    _histMenu.push(MenuItemEnum::QUIT);
    _histMenu.push(MenuItemEnum::MENU_ROOT);

    _p_MenuMgr->SetBackground(_p_SceneBackground);
    if (err != NULL) {
        return err;
    }
    OptionDelegator optionDelegator = prepOptionDelegator();

    err = _p_OptGfx->Initialize(_p_Screen, screenUpdater, optionDelegator,
                                _p_Window);
    if (err) {
        return err;
    }
    _lastMainLoopticks = SDL_GetTicks();
    return NULL;
}

LPErrInApp AppGfx::ChangeSceneBackground(SDL_Surface** ppSceneBackground) {
    LPErrInApp err;
    err = loadSceneBackground();
    if (err != NULL) {
        return err;
    }
    _p_MenuMgr->SetBackground(_p_SceneBackground);

    *ppSceneBackground = _p_SceneBackground;
    return NULL;
}

LPErrInApp AppGfx::loadSceneBackground() {
    if (_p_SceneBackground != NULL) {
        SDL_DestroySurface(_p_SceneBackground);
        _p_SceneBackground = NULL;
    }
    if (_p_GameSettings->BackgroundType != BackgroundTypeEnum::Black) {
        std::string strFileName;
        if (_p_GameSettings->BackgroundType ==
            BackgroundTypeEnum::Commessaggio) {
            strFileName = g_lpszImageSplashComm;
        } else if (_p_GameSettings->BackgroundType ==
                   BackgroundTypeEnum::Mantova) {
            strFileName = g_lpszImageSplashMantova;
        } else {
            return ERR_UTIL::ErrorCreate("Backgound Type %d not supported\n",
                                         _p_GameSettings->BackgroundType);
        }
        SDL_IOStream* srcBack = SDL_IOFromFile(strFileName.c_str(), "rb");

        if (srcBack == 0) {
            return ERR_UTIL::ErrorCreate("Unable to load %s background image\n",
                                         strFileName.c_str());
        }
        _p_SceneBackground = IMG_LoadTyped_IO(srcBack, false, "JPG");
        if (_p_SceneBackground == 0) {
            return ERR_UTIL::ErrorCreate("Unable to create splash");
        }
        SDL_CloseIO(srcBack);
    } else {
        _p_SceneBackground = GFX_UTIL::SDL_CreateRGBSurface(
            _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

        SDL_Rect clipRect;
        SDL_GetSurfaceClipRect(_p_SceneBackground, &clipRect);
        SDL_FillSurfaceRect(
            _p_SceneBackground, &clipRect,
            SDL_MapRGB(SDL_GetPixelFormatDetails(_p_SceneBackground->format),
                       NULL, 0, 0, 0));
    }

    return NULL;
}

LPErrInApp AppGfx::createWindow() {
    TRACE_DEBUG("createWindow\n");
    SDL_WindowFlags flagwin;
    if (_p_Window != NULL) {
        _p_Window = NULL;
        SDL_DestroyWindow(_p_Window);
    }
    if (_p_Screen != NULL) {
        SDL_DestroySurface(_p_Screen);
        _p_Screen = NULL;
    }
    if (_fullScreen) {
        flagwin = SDL_WINDOW_FULLSCREEN;
    } else {
        flagwin = SDL_WINDOW_RESIZABLE;
    }
#ifdef ANDROID
    flagwin = SDL_WINDOW_FULLSCREEN;
#endif

    TRACE_DEBUG("Create window with width %d, height %d\n",
                g_ResolutionMgr.displayWidth, g_ResolutionMgr.displayHeight);
    _p_Window = SDL_CreateWindow(_p_GameSettings->GameName.c_str(),
                                 g_ResolutionMgr.displayWidth,
                                 g_ResolutionMgr.displayHeight, flagwin);

    if (_p_Window == NULL) {
        return ERR_UTIL::ErrorCreate("Error SDL_CreateWindow: %s\n",
                                     SDL_GetError());
    }
    LPErrInApp err = _p_GameSettings->SetDisplaySize(
        g_ResolutionMgr.displayWidth, g_ResolutionMgr.displayHeight);
    if (err != NULL) {
        return err;
    }
    //_screenH = _p_GameSettings->GetScreenHeight();
    //_screenW = _p_GameSettings->GetScreenWidth();

    _p_sdlRenderer = SDL_CreateRenderer(_p_Window, NULL);

    if (_p_sdlRenderer == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot create renderer: %s\n",
                                     SDL_GetError());
    }

    _p_Screen = GFX_UTIL::SDL_CreateRGBSurface(
        g_ResolutionMgr.targetWidth, g_ResolutionMgr.targetHeight, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    if (_p_Screen == NULL) {
        return ERR_UTIL::ErrorCreate("Error SDL_CreateRGBSurface: %s\n",
                                     SDL_GetError());
    }
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
    }
    _p_ScreenTexture = SDL_CreateTexture(
        _p_sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        g_ResolutionMgr.targetWidth, g_ResolutionMgr.targetHeight);
    if (_p_ScreenTexture == NULL) {
        return ERR_UTIL::ErrorCreate("Error SDL_CreateTexture: %s\n",
                                     SDL_GetError());
    }
    TRACE_DEBUG("createWindow - Success\n");
    return NULL;
}

void AppGfx::terminate() {
    delete _p_CreditsView;
    delete _p_OptGfx;

    SDL_ShowCursor();

    if (_p_Screen != NULL) {
        SDL_DestroySurface(_p_Screen);
        _p_Screen = NULL;
    }
    if (_p_SceneBackground) {
        SDL_DestroySurface(_p_SceneBackground);
        _p_SceneBackground = NULL;
    }
    if (_p_CreditTitle) {
        SDL_DestroySurface(_p_CreditTitle);
        _p_CreditTitle = NULL;
    }
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
    }
    if (_p_MenuMgr != NULL) {
        delete _p_MenuMgr;
        _p_MenuMgr = NULL;
    }

    if (_p_SolitarioGfx != NULL) {
        delete _p_SolitarioGfx;
        _p_SolitarioGfx = NULL;
    }

    delete _p_HighScore;
    _p_GameSettings->TerminateMusicManager();

    SDL_DestroyWindow(_p_Window);
    SDL_Quit();
}

LPErrInApp AppGfx::loadProfile() {
    bool dirCreated;
    char dirpath[PATH_MAX];
    snprintf(dirpath, sizeof(dirpath), "%s", GAMESET::GetHomeFolder());
    LPErrInApp err = GAMESET::CreateHomeFolderIfNotExists(dirCreated);
    if (err != NULL) {
        return err;
    }
    if (dirCreated) {
        TRACE("Created dir %s\n", dirpath);
    }
    _p_GameSettings->SettingsDir = dirpath;

    TRACE("Load profile\n");
    err = _p_GameSettings->LoadSettings();
    if (err != NULL) {
        TRACE("Ignore settings because error: %s\n", err->ErrorText.c_str());
    }
    return NULL;
}

LPErrInApp fncBind_LeaveMenu(void* self) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->LeaveMenu();
}

LPErrInApp fncBind_EnterMenu(void* self, MenuItemEnum menuItem) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->EnterMenu(menuItem);
}

LPErrInApp fncBind_SettingsChanged(void* self, bool backGroundChanged,
                                   bool languageChanged) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->SettingsChanged(backGroundChanged, languageChanged);
}

LPErrInApp fncBind_ChangeSceneBackground(void* self,
                                         SDL_Surface** ppSceneBackground) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->ChangeSceneBackground(ppSceneBackground);
}

void fncBind_UpdateScreen(void* self, SDL_Surface* pScreen) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->UpdateScreen(pScreen);
}

void fncBind_RenderTexture(void* self, SDL_Texture* pScreenTexture) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->RenderTexture(pScreenTexture);
}

MenuDelegator AppGfx::prepMenuDelegator() {
    // Use only static otherwise you loose it
    static VMenuDelegator const tc = {.LeaveMenu = (&fncBind_LeaveMenu),
                                      .EnterMenu = (&fncBind_EnterMenu)};

    return (MenuDelegator){.tc = &tc, .self = this};
}

UpdateScreenCb AppGfx::prepScreenUpdater() {
    static VUpdateScreenCb const tc = {
        .UpdateScreen = (&fncBind_UpdateScreen),
        .RenderTexture = (&fncBind_RenderTexture)};
    return (UpdateScreenCb){.tc = &tc, .self = this};
}

OptionDelegator AppGfx::prepOptionDelegator() {
    // Use only static otherwise you loose it
    static VOptionDelegator const tc = {
        .SettingsChanged = (&fncBind_SettingsChanged),
        .ChangeSceneBackground = (&fncBind_ChangeSceneBackground)};

    return (OptionDelegator){.tc = &tc, .self = this};
}

LPErrInApp AppGfx::LeaveMenu() {
    clearBackground();
    _histMenu.pop();
    return NULL;
}

LPErrInApp AppGfx::EnterMenu(MenuItemEnum menuItem) {
    _histMenu.push(menuItem);
    LPErrInApp err;
    switch (menuItem) {
        case MenuItemEnum::MENU_GAME:
            err = startGameLoop();
            if (err != NULL)
                return err;
            break;
            break;
        case MenuItemEnum::MENU_HELP:
            err = showHelp();
            if (err != NULL)
                return err;
            break;
        case MenuItemEnum::MENU_CREDITS:
            err = showCredits();
            if (err != NULL)
                return err;
            break;
        case MenuItemEnum::MENU_HIGHSCORE:
            err = showHighScore();
            if (err != NULL)
                return err;
            break;
        case MenuItemEnum::MENU_OPTIONS:
            err = showGeneralOptions();
            if (err != NULL)
                return err;
            break;
        default:
            // Nothing to do
            break;
    }
    return NULL;
}

void AppGfx::transformMouseToTarget(int mouseX, int mouseY,
                                    SDL_Point* pTargetPos) {
    if (g_ResolutionMgr.scale == 1.0) {
        pTargetPos->x = mouseX;
        pTargetPos->y = mouseY;
        return;
    }

    pTargetPos->x = -1;
    pTargetPos->y = -1;

    // Check if mouse is within the game viewport
    if (mouseX < g_ResolutionMgr.viewport.x ||
        mouseY < g_ResolutionMgr.viewport.y ||
        mouseX > g_ResolutionMgr.viewport.x + g_ResolutionMgr.viewport.w ||
        mouseY > g_ResolutionMgr.viewport.y + g_ResolutionMgr.viewport.h) {
        return;  // Mouse outside game area
    }

    // Convert to target coordinates
    pTargetPos->x =
        (int)((mouseX - g_ResolutionMgr.viewport.x) / g_ResolutionMgr.scale);
    pTargetPos->y =
        (int)((mouseY - g_ResolutionMgr.viewport.y) / g_ResolutionMgr.scale);

    // Clamp to valid range
    pTargetPos->x =
        SDL_clamp(pTargetPos->x, 0, g_ResolutionMgr.targetWidth - 1);
    pTargetPos->y =
        SDL_clamp(pTargetPos->y, 0, g_ResolutionMgr.targetHeight - 1);
}

LPErrInApp AppGfx::MainLoopEvent(SDL_Event* pEvent, SDL_AppResult& res) {
    LPErrInApp err;
    res = SDL_APP_CONTINUE;
    SDL_Point targetPos = {-1, -1};

    switch (pEvent->type) {
        case SDL_EVENT_MOUSE_MOTION: {
            transformMouseToTarget(pEvent->motion.x, pEvent->motion.y,
                                   &targetPos);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            transformMouseToTarget(pEvent->button.x, pEvent->button.y,
                                   &targetPos);
            break;
        }

        case SDL_EVENT_WINDOW_RESIZED:
            resolutionMgr_UpdateViewport(g_ResolutionMgr, pEvent->window.data1,
                                         pEvent->window.data2);
            break;
        default:
            break;
    }

    switch (_histMenu.top()) {
        case MenuItemEnum::MENU_ROOT:
            err = _p_MenuMgr->HandleRootMenuEvent(pEvent, targetPos);
            if (err != NULL)
                return err;
            break;

        case MenuItemEnum::MENU_GAME:
            err = _p_SolitarioGfx->HandleEvent(pEvent, targetPos);
            if (err != NULL)
                return err;
            break;

        case MenuItemEnum::MENU_HELP:
            break;

        case MenuItemEnum::MENU_CREDITS:
            err = _p_CreditsView->HandleEvent(pEvent);
            if (err != NULL)
                return err;
            break;

        case MenuItemEnum::MENU_HIGHSCORE:
            err = _p_HighScore->HandleEvent(pEvent);
            if (err != NULL)
                return err;
            break;

        case MenuItemEnum::MENU_OPTIONS:
            err = _p_OptGfx->HandleEvent(pEvent, targetPos);
            if (err != NULL)
                return err;
            break;

        case MenuItemEnum::QUIT:
            TRACE("Quit Menu \n");
            res = SDL_APP_SUCCESS;
            break;
        default:
            TRACE("Exit application because outside of menu\n");
            res = SDL_APP_SUCCESS;
            break;
    }
    return NULL;
}

LPErrInApp AppGfx::MainLoopIterate() {
    // Do frame pacing, here is important to avoid freezing
    // a storm of iterate events (iteration too fast or too slow)
    Uint64 now = SDL_GetTicks();
    Uint64 elapsed = now - _lastMainLoopticks;

    Uint64 frames = 16;  // 16 is ~60 FPS
    if (elapsed < frames) {
        SDL_Delay(frames - elapsed);
        return NULL;
    }
    _lastMainLoopticks = now;
    if (elapsed > 100) {
        TRACE_DEBUG("WARN elapsed over %d\n", elapsed);
    }

    LPErrInApp err;
    bool done = false;

    switch (_histMenu.top()) {
        case MenuItemEnum::MENU_ROOT:
            if (!_p_MusicManager->IsPlayingMusic()) {
                _p_MusicManager->PlayMusic(MusicManager::MUSIC_INIT_SND,
                                           MusicManager::LOOP_ON);
            }
            err = _p_MenuMgr->HandleRootMenuIterate();
            if (err != NULL)
                return err;
            break;

        case MenuItemEnum::MENU_GAME:
            err = _p_SolitarioGfx->HandleIterate(done);
            if (err != NULL)
                return err;
            if (done) {
                backToMenuRootWithMusic();
            }
            break;

        case MenuItemEnum::MENU_HELP:
            break;

        case MenuItemEnum::MENU_CREDITS:
            err = _p_CreditsView->HandleIterate(done);
            if (err != NULL)
                return err;
            if (done) {
                backToMenuRootWithMusic();
            }
            break;

        case MenuItemEnum::MENU_HIGHSCORE:
            err = _p_HighScore->HandleIterate(done);
            if (err != NULL)
                return err;
            if (done) {
                backToMenuRootWithMusic();
            }
            break;

        case MenuItemEnum::MENU_OPTIONS:
            err = _p_OptGfx->HandleIterate(done);
            if (err != NULL)
                return err;
            if (done) {
                backToMenuRootSameMusic();
            }
            break;
        case MenuItemEnum::NOTHING:
            // Nothing to render
            break;
        case MenuItemEnum::QUIT:
            // Nothing to render
            break;
        default:
            break;
    }
    return NULL;
}

LPErrInApp AppGfx::startGameLoop() {
    TRACE("Start Game Loop, the game is the solitario\n");
    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(600);
    }

    LPErrInApp err;
    if (_p_SolitarioGfx != NULL) {
        delete _p_SolitarioGfx;
    }
    _p_SolitarioGfx = new SolitarioGfx();
    UpdateScreenCb screenUpdater = prepScreenUpdater();
    err = _p_SolitarioGfx->Initialize(_p_Screen, screenUpdater, _p_Window,
                                      _p_SceneBackground, _p_HighScore);
    if (err != NULL)
        return err;

    return _p_SolitarioGfx->Show();
}

LPErrInApp AppGfx::showHelp() {
    TRACE("Show Help\n");
    const char* cmd = NULL;
    char cmdpath[PATH_MAX];
#if PLATFORM_WINDOWS
    cmd = "start";
    snprintf(cmdpath, sizeof(cmdpath), "%s .\\%s", cmd, g_lpszHelpFileName);
#elif PLATFORM_ANDROID
    // TODO open the pdf file
    TRACE_DEBUG("Wanna open file %s\n", g_lpszHelpFileName);
#else
    cmd = "zathura";
    snprintf(cmdpath, sizeof(cmdpath), "%s ./%s", cmd, g_lpszHelpFileName);
#endif
#ifndef ANDROID
    system(cmdpath);
#endif
    LeaveMenu();
    return NULL;
}

LPErrInApp AppGfx::showHighScore() {
    TRACE("Show HighScore\n");
    if (_p_HighScore->IsOngoing()) {
        return ERR_UTIL::ErrorCreate("Credit already started");
    }
    if (_p_HighScore->IsOngoing()) {
        return NULL;
    }
    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(600);
    }
    UpdateScreenCb screenUpdater = prepScreenUpdater();
    _p_HighScore->Show(_p_Screen, _p_CreditTitle, screenUpdater);

    return NULL;
}

LPErrInApp AppGfx::showCredits() {
    TRACE("Show Credits\n");
    if (_p_CreditsView->IsOngoing()) {
        return ERR_UTIL::ErrorCreate("Credit already started");
    }
    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(600);
    }
    UpdateScreenCb screenUpdater = prepScreenUpdater();
    _p_CreditsView->Show(_p_Screen, _p_CreditTitle, screenUpdater);

    return NULL;
}

LPErrInApp AppGfx::showGeneralOptions() {
    TRACE("Show general Options\n");
    if (_p_OptGfx->IsOngoing()) {
        return ERR_UTIL::ErrorCreate("General Options already started");
    }

    LPLanguages pLanguages = _p_GameSettings->GetLanguageMan();
    STRING caption = pLanguages->GetStringId(Languages::ID_MEN_OPTIONS);
    LPErrInApp err = _p_OptGfx->Show(_p_SceneBackground, caption);
    if (err) {
        return err;
    }
    return NULL;
}

void AppGfx::backToMenuRootWithMusic() {
    TRACE("Back to root menu\n");
    LeaveMenu();
    _p_MusicManager->PlayMusic(MusicManager::MUSIC_INIT_SND,
                               MusicManager::LOOP_ON);
    _p_HighScore->Reset();
    _p_CreditsView->Reset();
}

void AppGfx::backToMenuRootSameMusic() {
    TRACE("Back to root menu same music\n");
    LeaveMenu();
    _p_OptGfx->Reset();
}

LPErrInApp AppGfx::SettingsChanged(bool backGroundChanged,
                                   bool languageChanged) {
    TRACE_DEBUG("Settings changed, background: %s, language: %s\n",
                backGroundChanged ? "true" : "false",
                languageChanged ? "true" : "false");
    return NULL;
}

void AppGfx::clearBackground() {
    TRACE_DEBUG("Clear background\n");
    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    SDL_FillSurfaceRect(_p_Screen, &clipRect,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));
    updateScreenTexture();
}

void AppGfx::UpdateScreen(SDL_Surface* pScreen) {
    _p_Screen = pScreen;
    updateScreenTexture();
}

void AppGfx::RenderTexture(SDL_Texture* pScreenTexture) {
    if (g_ResolutionMgr.scale == 1.0) {
        SDL_RenderTexture(_p_sdlRenderer, pScreenTexture, NULL, NULL);
    } else {
        SDL_FRect destRect = {
            (g_ResolutionMgr.displayWidth -
             g_ResolutionMgr.targetWidth * g_ResolutionMgr.scale) *
                0.5f,
            (g_ResolutionMgr.displayHeight -
             g_ResolutionMgr.targetHeight * g_ResolutionMgr.scale) *
                0.5f,
            (float)g_ResolutionMgr.displayWidth,
            (float)g_ResolutionMgr.displayHeight};
        SDL_RenderTexture(_p_sdlRenderer, pScreenTexture, NULL, &destRect);
    }
}

void AppGfx::updateScreenTexture() {
    if (g_ResolutionMgr.isDev) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "View: %dx%d (Full: %dx%d) | Scale: %.1f%%",
                 g_ResolutionMgr.displayWidth, g_ResolutionMgr.displayHeight,
                 g_ResolutionMgr.targetWidth, g_ResolutionMgr.targetHeight,
                 g_ResolutionMgr.scale * 100.0f);
        GFX_UTIL::DrawString(_p_Screen, buffer, 10, 10, GFX_UTIL_COLOR::Red,
                             _p_GameSettings->GetFontAriblk());
    }
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    SDL_RenderClear(_p_sdlRenderer);
    // SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    RenderTexture(_p_ScreenTexture);
    SDL_RenderPresent(_p_sdlRenderer);
}

void AppGfx::ParseCmdLine(int argc, char* argv[], SDL_AppResult& res) {
    res = SDL_APP_CONTINUE;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("Solitario version %s (c) 2004-2026 Invido.it\n", VERSION);
            res = SDL_APP_SUCCESS;
        } else if (std::string(argv[i]) == "--language" && i + 1 < argc) {
            std::string language = argv[i + 1];
            TRACE_DEBUG("[ParseCmdLine] Startup language recognized %s \n",
                        language.c_str());
            i++;  // Skip the value in the next loop iteration
            LPGameSettings pGameSettings = GameSettings::GetSettings();
            if (language == "italian") {
                TRACE_DEBUG("[ParseCmdLine] change the language to italian");
                pGameSettings->CurrentLanguage = Languages::eLangId::LANG_ITA;
                pGameSettings->SetCurrentLang();
            }
        } else if (strcmp(argv[i], "--portrait") == 0) {
            TRACE("[ParseCmdLine] portrait mode recognized \n");
            LPGameSettings pGameSettings = GameSettings::GetSettings();
            pGameSettings->SetPortraitMode(true);
        } else if (strcmp(argv[i], "--fullportrait") == 0) {
            TRACE("[ParseCmdLine] full portrait mode recognized \n");
            LPGameSettings pGameSettings = GameSettings::GetSettings();
            pGameSettings->SetFullPortraitMode(true);
        }  
        else if (strcmp(argv[i], "--web") == 0) {
            TRACE("[ParseCmdLine] web mode recognized \n");
            LPGameSettings pGameSettings = GameSettings::GetSettings();
            pGameSettings->SetWebMode(true);
        }
        else {
            printf("unknown option: %s\n", argv[i]);
            printf("\nUsage: %s --version \n", argv[0]);
            res = SDL_APP_SUCCESS;
        }
    }
}
