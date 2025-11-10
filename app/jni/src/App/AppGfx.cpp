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

AppGfx::AppGfx() {
    _p_Window = NULL;
    _p_ScreenTexture = NULL;
    _p_SolitarioGfx = NULL;
    _p_SceneBackground = NULL;
    _p_Screen = NULL;
    _screenW = 1024;
    _screenH = 768;
    _Bpp = 0;
    _p_MusicManager = 0;
    _p_HighScore = 0;
    _p_CreditTitle = 0;
    _fullScreen = false;
    _p_GameSettings = GameSettings::GetSettings();
}

AppGfx::~AppGfx() { terminate(); }

LPErrInApp AppGfx::Init() {
    TRACE("Init App\n");
    _p_GameSettings->GameName = "Solitario";
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
        if (SDL_Init(0) < 0) {
            return ERR_UTIL::ErrorCreate("Couldn't initialize SDL: %s\n",
                                         SDL_GetError());
        }
    }
    err = createWindow();
    if (err != NULL) {
        return err;
    }

    _p_HighScore = new HighScore();
    _p_HighScore->Load();

    _p_GameSettings->SetCurrentLang();
    Languages* pLanguages = _p_GameSettings->GetLanguageMan();

    if (TTF_Init() == -1) {
        return ERR_UTIL::ErrorCreate("Font init error");
    }
    if (_p_GameSettings->NeedScreenMagnify()) {
        _p_GameSettings->UseBigFontSize();
    }

    const char* title = pLanguages->GetCStringId(Languages::ID_SOLITARIO);
    SDL_SetWindowTitle(_p_Window, title);

    SDL_Surface* psIcon = SDL_LoadBMP(g_lpszIconProgFile);
    if (psIcon == 0) {
        return ERR_UTIL::ErrorCreate("Icon not found");
    }
    // SDL_SetColorKey(psIcon, true, SDL_MapRGB(psIcon->format, 0, 128, 0));
    // SDL2
    SDL_SetSurfaceColorKey(
        psIcon, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails(psIcon->format), NULL, 0, 128, 0));

    SDL_SetWindowIcon(_p_Window, psIcon);

    _p_CreditTitle = IMG_Load(g_lpszTitleFile);
    if (_p_CreditTitle == 0) {
        return ERR_UTIL::ErrorCreate("Title image not found");
    }
    err = loadSceneBackground();
    if (err != NULL) {
        return err;
    }
    err = _p_GameSettings->LoadFonts();
    if (err != NULL) {
        return err;
    }

    clearBackground();

    _p_MusicManager = _p_GameSettings->InitMusicManager();
    err = _p_MusicManager->LoadMusicRes();
    return err;
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
        // SDL_RWops *srcBack = SDL_RWFromFile(strFileName.c_str(), "rb"); SDL2
        SDL_IOStream* srcBack = SDL_IOFromFile(strFileName.c_str(), "rb");

        if (srcBack == 0) {
            return ERR_UTIL::ErrorCreate("Unable to load %s background image\n",
                                         strFileName.c_str());
        }
        //_p_SceneBackground = IMG_LoadJPG_RW(srcBack); SDL 2
        _p_SceneBackground = IMG_LoadTyped_IO(srcBack, false, "JPG");
        if (_p_SceneBackground == 0) {
            return ERR_UTIL::ErrorCreate("Unable to create splash");
        }
        SDL_CloseIO(srcBack);
    } else {
        // _p_SceneBackground = SDL_CreateRGBSurface(SDL_SWSURFACE,
        // _p_Screen->w,
        //                                           _p_Screen->h, 32, 0, 0, 0,
        //                                           0); SDL 2
        _p_SceneBackground = GFX_UTIL::SDL_CreateRGBSurface(
            _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

        // SDL_FillRect(_p_SceneBackground, &_p_SceneBackground->clip_rect,
        //              SDL_MapRGBA(_p_SceneBackground->format, 0, 0, 0, 0));
        //              SDL 2
        SDL_Rect clipRect;  // SDL 3
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
    // int flagwin = 0; SDL 2
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
        // flagwin = SDL_WINDOW_FULLSCREEN_DESKTOP; SDL 2
        flagwin = SDL_WINDOW_FULLSCREEN;
    } else {
        // flagwin = SDL_WINDOW_SHOWN; SDL 2
        flagwin = SDL_WINDOW_RESIZABLE;
    }
#ifdef ANDROID
    flagwin = SDL_WINDOW_FULLSCREEN;
#endif

    // _p_Window = SDL_CreateWindow(
    //     _p_GameSettings->GameName.c_str(), SDL_WINDOWPOS_UNDEFINED,
    //     SDL_WINDOWPOS_UNDEFINED, _screenW, _screenH, flagwin); SDL 2
    _p_Window = SDL_CreateWindow(_p_GameSettings->GameName.c_str(), _screenW,
                                 _screenH, flagwin);

    if (_p_Window == NULL) {
        return ERR_UTIL::ErrorCreate("Error SDL_CreateWindow: %s\n",
                                     SDL_GetError());
    }
    LPErrInApp err = _p_GameSettings->CalcDisplaySize(_screenW, _screenH);
    if (err != NULL) {
        return err;
    }
    _screenH = _p_GameSettings->GetScreenHeight();
    _screenW = _p_GameSettings->GetScreenWidth();

    _p_sdlRenderer =
        // SDL_CreateRenderer(_p_Window, -1, SDL_RENDERER_ACCELERATED); SDL 2
        SDL_CreateRenderer(_p_Window, NULL);

    if (_p_sdlRenderer == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot create renderer: %s\n",
                                     SDL_GetError());
    }

    // _p_Screen = SDL_CreateRGBSurface(0, _screenW, _screenH, 32, 0x00FF0000,
    //                                  0x0000FF00, 0x000000FF, 0xFF000000); SDL
    //                                  2
    _p_Screen = GFX_UTIL::SDL_CreateRGBSurface(
        _screenW, _screenH, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    if (_p_Screen == NULL) {
        return ERR_UTIL::ErrorCreate("Error SDL_CreateRGBSurface: %s\n",
                                     SDL_GetError());
    }
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
    }
    _p_ScreenTexture =
        SDL_CreateTexture(_p_sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, _screenW, _screenH);
    if (_p_ScreenTexture == NULL) {
        return ERR_UTIL::ErrorCreate("Error SDL_CreateTexture: %s\n",
                                     SDL_GetError());
    }
    TRACE_DEBUG("createWindow - Success\n");
    return NULL;
}

LPErrInApp AppGfx::startGameLoop() {
    TRACE("Start Game Loop\n");
    _p_MusicManager->StopMusic(700);

    LPErrInApp err;
    if (_p_SolitarioGfx != NULL) {
        delete _p_SolitarioGfx;
    }
    _p_SolitarioGfx = new SolitarioGfx();

    err = _p_SolitarioGfx->Initialize(_p_Screen, _p_sdlRenderer, _p_Window,
                                      _p_SceneBackground, _p_HighScore);
    if (err != NULL)
        return err;

    return _p_SolitarioGfx->StartGameLoop();
}

void AppGfx::terminate() {
    writeProfile();
    // SDL_ShowCursor(SDL_ENABLE);SDL2
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

    delete _p_SolitarioGfx;
    delete _p_HighScore;
    _p_SolitarioGfx = NULL;
    _p_GameSettings->TerminateMusicManager();
    _p_HighScore = NULL;

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
    return _p_GameSettings->LoadSettings();
}

LPErrInApp AppGfx::writeProfile() {
    TRACE("Save profile \n");
    return _p_GameSettings->SaveSettings();
}

void fncBind_LeaveMenu(void* self) {
    AppGfx* pApp = (AppGfx*)self;
    pApp->LeaveMenu();
}

void fncBind_SetNextMenu(void* self, MenuItemEnum menuItem) {
    AppGfx* pApp = (AppGfx*)self;
    pApp->SetNextMenu(menuItem);
}

LPErrInApp fncBind_SettingsChanged(void* self, bool backGroundChanged,
                                   bool languageChanged) {
    AppGfx* pApp = (AppGfx*)self;
    return pApp->SettingsChanged(backGroundChanged, languageChanged);
}

MenuDelegator AppGfx::prepMenuDelegator() {
    // Use only static otherwise you loose it
    static VMenuDelegator const tc = {
        .LeaveMenu = (&fncBind_LeaveMenu),
        .SetNextMenu = (&fncBind_SetNextMenu),
        .SettingsChanged = (&fncBind_SettingsChanged)};

    return (MenuDelegator){.tc = &tc, .self = this};
}

void AppGfx::LeaveMenu() {
    clearBackground();
    _histMenu.pop();
}

LPErrInApp AppGfx::SettingsChanged(bool backGroundChanged,
                                   bool languageChanged) {
    TRACE("Persist settings\n");
    if (backGroundChanged) {
        _backGroundChanged = true;
    }
    _p_GameSettings->SetCurrentLang();
    return writeProfile();
}

void AppGfx::clearBackground() {
    TRACE_DEBUG("Clear background\n");
    // SDL_FillRect(_p_Screen, &_p_Screen->clip_rect,
    //              SDL_MapRGBA(_p_Screen->format, 0, 0, 0, 0)); SDL 2
    SDL_Rect clipRect;  // SDL 3
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    SDL_FillSurfaceRect(_p_Screen, &clipRect,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));
    updateScreenTexture();
}

LPErrInApp AppGfx::MainLoop() {
    LPErrInApp err;
    bool quit = false;

    MenuMgr* pMenuMgr = new MenuMgr();
    MenuDelegator delegator = prepMenuDelegator();
    err = pMenuMgr->Initialize(_p_Screen, _p_sdlRenderer, _p_Window, delegator);
    if (err != NULL)
        goto error;

    // set main menu
    _histMenu.push(MenuItemEnum::QUIT);
    _histMenu.push(MenuItemEnum::MENU_ROOT);

    pMenuMgr->SetBackground(_p_SceneBackground);

    while (!quit && !_histMenu.empty()) {
        switch (_histMenu.top()) {
            case MenuItemEnum::MENU_ROOT:
                if (!_p_MusicManager->IsPlayingMusic()) {
                    _p_MusicManager->PlayMusic(MusicManager::MUSIC_INIT_SND,
                                               MusicManager::LOOP_ON);
                }
                err = pMenuMgr->HandleRootMenu();
                if (err != NULL)
                    goto error;
                break;

            case MenuItemEnum::MENU_GAME:
                err = startGameLoop();
                if (err != NULL)
                    goto error;
                LeaveMenu();
                break;

            case MenuItemEnum::MENU_HELP:
                err = showHelp();
                if (err != NULL)
                    goto error;
                break;

            case MenuItemEnum::MENU_CREDITS:
                err = showCredits();
                if (err != NULL)
                    goto error;
                break;

            case MenuItemEnum::MENU_HIGHSCORE:
                err = showHighScore();
                if (err != NULL)
                    goto error;
                break;

            case MenuItemEnum::MENU_OPTIONS:
                _backGroundChanged = false;
                err = showOptionGeneral();
                if (err != NULL)
                    goto error;
                if (_backGroundChanged) {
                    err = loadSceneBackground();
                    if (err)
                        goto error;
                    pMenuMgr->SetBackground(_p_SceneBackground);
                }
                break;

            case MenuItemEnum::QUIT:
            default:
                quit = true;
                break;
        }

        updateScreenTexture();
    }
    delete pMenuMgr;
    return NULL;
error:
    delete pMenuMgr;
    return err;
}

LPErrInApp AppGfx::showHelp() {
    const char* cmd = NULL;
    char cmdpath[PATH_MAX];
#ifdef WIN32
    cmd = "start";
    snprintf(cmdpath, sizeof(cmdpath), "%s .\\%s", cmd, g_lpszHelpFileName);
#endif
#ifdef ANDROID
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
    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(600);
    }
    _p_HighScore->Show(_p_Screen, _p_CreditTitle, _p_sdlRenderer);

    LeaveMenu();

    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(300);
        _p_MusicManager->PlayMusic(MusicManager::MUSIC_INIT_SND,
                                   MusicManager::LOOP_ON);
    }

    return NULL;
}

LPErrInApp AppGfx::showCredits() {
    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(600);
    }
    credits(_p_Screen, _p_CreditTitle, _p_sdlRenderer);
    LeaveMenu();
    _p_MusicManager->PlayMusic(MusicManager::MUSIC_INIT_SND,
                               MusicManager::LOOP_ON);
    return NULL;
}

LPErrInApp AppGfx::showOptionGeneral() {
    TRACE("Show option general\n");
    OptionsGfx optGfx;

    MenuDelegator delegator = prepMenuDelegator();
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();

    STRING caption = pLanguages->GetStringId(Languages::ID_MEN_OPTIONS);
    LPErrInApp err = optGfx.Initialize(_p_Screen, _p_sdlRenderer, delegator);
    if (err) {
        return err;
    }
    err = optGfx.Show(_p_SceneBackground, caption);
    if (err) {
        return err;
    }

    LeaveMenu();
    return NULL;
}

void AppGfx::updateScreenTexture() {
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    SDL_RenderClear(_p_sdlRenderer);
    SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);
}

void AppGfx::ParseCmdLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("Solitario version %s (c) 2004-2025 Invido.it\n", VERSION);
            exit(0);
        } else {
            printf("unknown option: %s\n", argv[i]);
            printf("\nUsage: %s --version \n", argv[0]);
            exit(1);
        }
    }
}
