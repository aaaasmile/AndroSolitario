#include "SolitarioGfx.h"

#include <SDL3/SDL_endian.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

#include "CompGfx/ButtonGfx.h"
#include "CompGfx/MesgBoxGfx.h"
#include "Config.h"
#include "CurrentTime.h"
#include "DeckLoader.h"
#include "Fading.h"
#include "GfxUtil.h"
#include "MusicManager.h"

const int MYIDQUIT = 0;
const int MYIDNEWGAME = 1;
const int MYIDTOGGLESOUND = 2;
const int MYIDTOGGLEFULLSCREEN = 3;

SolitarioGfx::SolitarioGfx() {
    _p_ScreenBackbufferDrag = NULL;
    _startdrag = false;
    _p_Dragface = NULL;
    _p_SceneBackground = NULL;
    _p_BtQuit = NULL;
    _p_BtNewGame = NULL;
    _p_BtToggleSound = NULL;
    _p_BtToggleFullscreen = NULL;
    _p_AlphaDisplay = NULL;
    _p_MsgBox = NULL;
    _p_currentTime = new CurrentTime();
    _p_FadeAction = new FadeAction();
    _state = READY_TO_START;
    _continueFnCb = NULL;
    _p_DropRegionForDrag = NULL;
    _p_CardStackForDrag = NULL;
    _isInitDrag = false;
    _doubleTapWait = 300;
    _scaleFactor = 0.0f;
    _lastUpTimestamp = 0;
    _fnUpdateScreen.tc = NULL;
    _fnUpdateScreen.self = NULL;
    _p_DeckLoader = new DeckLoader();
}

SolitarioGfx::~SolitarioGfx() {
    CleanUpRegion();
    clearSurface();
    clearAnimation();
    if (_p_BtNewGame != NULL) {
        delete _p_BtNewGame;
        _p_BtNewGame = NULL;
    }
    if (_p_BtQuit != NULL) {
        delete _p_BtQuit;
        _p_BtQuit = NULL;
    }
    if (_p_BtToggleSound != NULL) {
        delete _p_BtToggleSound;
        _p_BtToggleSound = NULL;
    }
    if (_p_BtToggleFullscreen != NULL) {
        delete _p_BtToggleFullscreen;
        _p_BtToggleFullscreen = NULL;
    }
    delete _p_currentTime;
    delete _p_FadeAction;
    delete _p_DeckLoader;
    if (_p_MsgBox != NULL) {
        delete _p_MsgBox;
        _p_MsgBox = NULL;
    }
}

void SolitarioGfx::clearSurface() {
    TRACE_DEBUG("clearSurface\n");
    if (_p_ScreenBackbufferDrag != NULL) {
        SDL_DestroySurface(_p_ScreenBackbufferDrag);
        _p_ScreenBackbufferDrag = NULL;
    }
    if (_p_AlphaDisplay != NULL) {
        SDL_DestroySurface(_p_AlphaDisplay);
        _p_AlphaDisplay = NULL;
    }
}

// command buttons
// -- quit --
static void fncBind_ButtonQuitClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtQuitClick();
}

ClickCb SolitarioGfx::prepClickQuitCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonQuitClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

// -- new game --
static void fncBind_ButtonNewGameClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtNewGameClick();
}

ClickCb SolitarioGfx::prepClickNewGameCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonNewGameClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

// -- sound on/off --
static void fncBind_ButtonToggleSoundClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtToggleSoundClick();
}

ClickCb SolitarioGfx::prepClickToggleSoundCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonToggleSoundClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

// -- fullscreen toggle --
static void fncBind_ButtonToggleFullscreenClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtToggleFullscreenClick();
}

ClickCb SolitarioGfx::prepClickToggleFullscreenCb() {
    static VClickCb const tc = {.Click =
                                    (&fncBind_ButtonToggleFullscreenClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

LPErrInApp SolitarioGfx::Initialize(SDL_Surface* pScreen,
                                    UpdateScreenCb& fnUpdateScreen,
                                    SDL_Window* pWindow,
                                    SDL_Surface* pSceneBackground,
                                    UpdateHighScoreCb& fnUpdateHighScore) {
    TRACE("Initialize Solitario\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    setDeckType(pGameSettings->DeckTypeVal);
    _p_MusicManager = pGameSettings->GetMusicManager();
    //_p_HighScore = pHighScore;
    _fnUpdateHighScore = fnUpdateHighScore;
    _sceneBackgroundIsBlack =
        pGameSettings->BackgroundType == BackgroundTypeEnum::Black;
    _p_FontBigText = pGameSettings->GetFontDjvBig();
    LPErrInApp err;
    _p_Screen = pScreen;
    _fnUpdateScreen = fnUpdateScreen;
    _p_Window = pWindow;

    _p_AlphaDisplay = GFX_UTIL::SDL_CreateRGBSurface(_p_Screen->w, _p_Screen->h,
                                                     32, 0, 0, 0, 0);
    if (_p_AlphaDisplay == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot create alpha display: %s\n",
                                     SDL_GetError());
    }

    _p_ScreenBackbufferDrag = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    _p_SceneBackground = pSceneBackground;
    if (_p_SceneBackground == 0) {
        return ERR_UTIL::ErrorCreate("Cannot create scene background: %s\n",
                                     SDL_GetError());
    }
    if (_deckType.IsPacType()) {
        TRACE("Deck Pac stuff\n");
        err = _p_DeckLoader->LoadCardPac(_deckType);
        if (err != NULL) {
            return err;
        }
        err = _p_DeckLoader->LoadSymbolsForPac(_deckType);
        if (err != NULL) {
            return err;
        }
    } else {
        return ERR_UTIL::ErrorCreate("Only pac file supported");
    }

    initButtons();
    _p_BtToggleSound->SetVisibleState(ButtonGfx::INVISIBLE);
    _p_BtQuit->SetVisibleState(ButtonGfx::INVISIBLE);
    _p_BtNewGame->SetVisibleState(ButtonGfx::INVISIBLE);
    _p_BtToggleFullscreen->SetVisibleState(ButtonGfx::INVISIBLE);

    TRACE_DEBUG("Solitario initialized \n");
    return NULL;
}

LPErrInApp SolitarioGfx::OnResize(SDL_Surface* pScreen) {
    TRACE_DEBUG("[SolitarioGfx::OnResize] screen w=%d, h=%d \n", pScreen->w,
                pScreen->h);
    _p_Screen = pScreen;

    if (_p_AlphaDisplay != NULL) {
        SDL_DestroySurface(_p_AlphaDisplay);
    }
    _p_AlphaDisplay = GFX_UTIL::SDL_CreateRGBSurface(_p_Screen->w, _p_Screen->h,
                                                     32, 0, 0, 0, 0);
    if (_p_AlphaDisplay == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot recreate alpha display: %s\n",
                                     SDL_GetError());
    }

    if (_p_ScreenBackbufferDrag != NULL) {
        SDL_DestroySurface(_p_ScreenBackbufferDrag);
    }
    _p_ScreenBackbufferDrag = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    initButtons();
    repositionRegions(false);

    DrawStaticScene();

    return NULL;
}

void SolitarioGfx::initButtons() {
    SDL_Rect rctBt1 = {0, 0, 0, 0};
    int btw = 120;
    int btwSymb = 60;
    int bth = 34;
    int btoffsetY = 70;
    int btintraX = 30;
    int soundIntraX = 70;

    int buttons_w = 2 * btw + btwSymb + 2 * btintraX + soundIntraX;
    int btposx = (_p_Screen->w - buttons_w) / 2;

    LPGameSettings pGameSettings = GameSettings::GetSettings();

    // Sound Toggle
    rctBt1.w = btwSymb;
    rctBt1.h = bth;
    rctBt1.x = btposx;
    rctBt1.y = _p_Screen->h - btoffsetY;
    if (_p_BtToggleSound == NULL) {
        _p_BtToggleSound = new ButtonGfx();
    }
    ClickCb cbBtToggleSound = prepClickToggleSoundCb();
    _p_BtToggleSound->InitializeAsSymbol(&rctBt1, _p_Screen,
                                         pGameSettings->GetFontSymb(),
                                         MYIDTOGGLESOUND, cbBtToggleSound);

    // button Quit
    if (_p_BtQuit == NULL) {
        _p_BtQuit = new ButtonGfx();
    }
    ClickCb cbBtQuit = prepClickQuitCb();
    rctBt1.x = rctBt1.x + rctBt1.w + soundIntraX;
    rctBt1.w = btw;
    _p_BtQuit->Initialize(&rctBt1, _p_Screen, _p_FontBigText, MYIDQUIT,
                          cbBtQuit);

    // button new game
    if (_p_BtNewGame == NULL) {
        _p_BtNewGame = new ButtonGfx();
    }
    ClickCb cbBtNewGame = prepClickNewGameCb();
    rctBt1.x = rctBt1.x + rctBt1.w + btintraX;
    rctBt1.w = btw;
    _p_BtNewGame->Initialize(&rctBt1, _p_Screen, _p_FontBigText, MYIDNEWGAME,
                             cbBtNewGame);

    // Fullscreen Toggle
    if (_p_BtToggleFullscreen == NULL) {
        _p_BtToggleFullscreen = new ButtonGfx();
    }
    ClickCb cbBtToggleFullscreen = prepClickToggleFullscreenCb();
    rctBt1.x = (rctBt1.x + rctBt1.w) + btintraX;
    rctBt1.w = btwSymb;
    rctBt1.h = bth;
    _p_BtToggleFullscreen->InitializeAsSymbol(
        &rctBt1, _p_Screen, pGameSettings->GetFontSymb(), MYIDTOGGLEFULLSCREEN,
        cbBtToggleFullscreen);
}

void SolitarioGfx::InitAllCoords(bool initFaces) {
    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        vir->InitCardCoords();
        if (initFaces)
            vir->InitCardFaces();
    }
}

LPErrInApp SolitarioGfx::DrawCardStack(LPCardRegionGfx pcardRegion) {
    return DrawCardStack(_p_Screen, pcardRegion);
}

LPErrInApp SolitarioGfx::DrawCardStack(SDL_Surface* s,
                                       LPCardRegionGfx pcardRegion) {
    if (pcardRegion == NULL) {
        return ERR_UTIL::ErrorCreate("DrawCardStack region is NULL");
    }

    LPErrInApp err;
    if (!pcardRegion->IsVisible())
        return NULL;

    float scaleFactor = pcardRegion->GetScaleFactor();

    if (pcardRegion->IsEmpty()) {
        DrawSymbol(pcardRegion->X(), pcardRegion->Y(), pcardRegion->Symbol(), s,
                   scaleFactor);
    } else {
        for (int i = 0; i < pcardRegion->Size(); i++) {
            LPCardGfx pCard = pcardRegion->Item(i);
            if (pCard->IsFaceUp()) {
                err = DrawCard(pCard, s, scaleFactor);
            } else {
                err = DrawCardBack(pCard->X(), pCard->Y(), s, scaleFactor);
            }
            if (err != NULL) {
                return err;
            }
        }
    }
    return NULL;
}

void SolitarioGfx::CreateRegion(RegionType rtid, unsigned int attribs,
                                unsigned int amode, int dmode, int symbol,
                                int x, int y, int xoffset, int yoffset) {
    CardRegionGfx* cr = new CardRegionGfx(
        rtid, attribs, amode, dmode, symbol, x, y, xoffset, yoffset, _deckType,
        _p_DeckLoader->GetCardWidth(), _p_DeckLoader->GetCardHeight());
    _cardRegionList.push_back(*cr);
}

bool SolitarioGfx::DeleteRegion(LPCardRegionGfx pRegion) {
    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        if (&(*vir) == pRegion) {
            _cardRegionList.erase(vir);
            return true;
        }
    }
    return false;
}

LPCardRegionGfx SolitarioGfx::SelectRegionOnPoint(int x, int y) {
    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        if (vir->PtInStack(x, y)) {
            _p_selectedCardRegion = &(*vir);
            return _p_selectedCardRegion;
        }
    }
    return NULL;
}

void SolitarioGfx::CleanUpRegion() {
    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        vir->CleanUp();
        vir->Clear();
    }
}

LPErrInApp SolitarioGfx::InitDrag(int x, int y, bool& isInitDrag,
                                  LPCardRegionGfx pSrcRegion) {
    return InitDrag(NULL, x, y, isInitDrag, pSrcRegion);
}

typedef struct initDragInfo {
    int x;
    int y;
    LPCardRegionGfx pSrcRegion;
}* LPinitDragInfo;
static LPinitDragInfo g_pInitDragInfo = NULL;

LPErrInApp SolitarioGfx::InitDrag(LPCardStackGfx pCargoStack, int x, int y,
                                  bool& isInitDrag,
                                  LPCardRegionGfx pSrcRegion) {
    TRACE_DEBUG("  InitDrag - start \n");
    isInitDrag = false;
    if (g_pInitDragInfo != NULL) {
        delete g_pInitDragInfo;
    }
    g_pInitDragInfo = new initDragInfo();
    g_pInitDragInfo->x = x;
    g_pInitDragInfo->y = y;
    g_pInitDragInfo->pSrcRegion = pSrcRegion;

    if (pCargoStack == NULL) {
        if (_p_selectedCardRegion->IsEmpty()) {
            return NULL;
        }
        int idx = _p_selectedCardRegion->GetClickedCard(x, y);
        if (idx == -1) {
            return NULL;  // no card found
        }
        int dm = _p_selectedCardRegion->GetDragMode();
        switch (dm) {
            case CRD_DRAGTOP: {
                if (_p_selectedCardRegion->Size() - 1 == idx)
                    _dragStack.PushCard(_p_selectedCardRegion->PopCard());
                else
                    return NULL;
                break;
            }
            case CRD_DRAGFACEUP: {
                if (_p_selectedCardRegion->IsCardFaceUp(idx)) {
                    LPCardStackGfx pStack = _p_selectedCardRegion->PopStack(
                        _p_selectedCardRegion->Size() - idx);
                    _dragStack.PushStack(pStack);
                    delete pStack;
                } else
                    return NULL;
                break;
            }
            default: {
                return ERR_UTIL::ErrorCreate(
                    "InitDrag error: Drag mode %d not supported\n", dm);
            }
        }
    } else
        _dragStack.PushStack(pCargoStack);

    _p_selectedCardRegion->InitCardCoords();
    isInitDrag = true;
    _state = SolitarioGfx::INITDRAG_STEP1;
    TRACE_DEBUG("  InitDrag - next step is 1 \n");
    return NULL;
}

LPErrInApp SolitarioGfx::InitDragContinueIterate() {
    if (_state == SolitarioGfx::INITDRAG_STEP1) {
        TRACE_DEBUG("[INITDRAG_STEP1]  DrawStaticScene \n");
        DrawStaticScene();
        _state = SolitarioGfx::INITDRAG_STEP2;
        return NULL;
    }

    if (_state == SolitarioGfx::INITDRAG_STEP2) {
        TRACE_DEBUG("[Step2]  InitDrag - DrawStaticScene - end \n");
        CardRegionGfx dragRegion(
            RegionType::RT_DRAG_REGION,
            _p_selectedCardRegion->GetAttributes() | CRD_FACEUP, 0, 0, 0, 0, 0,
            _p_selectedCardRegion->GetxOffset(),
            _p_selectedCardRegion->GetyOffset(), _deckType,
            _p_DeckLoader->GetCardWidth(), _p_DeckLoader->GetCardHeight());
        dragRegion.SetScaleFactor(_scaleFactor);

        dragRegion.PushStack(&_dragStack);

        _dragPileInfo.x = _dragStack.First()->X();
        _dragPileInfo.y = _dragStack.First()->Y();
        _dragPileInfo.width = dragRegion.GetStackWidth();
        _dragPileInfo.height = dragRegion.GetStackHeight();
        _dragPileInfo.pSrcRegion = g_pInitDragInfo->pSrcRegion;

        if (_p_Dragface != NULL) {
            SDL_DestroySurface(_p_Dragface);
        }

        _p_Dragface = GFX_UTIL::SDL_CreateRGBSurface(
            _dragPileInfo.width, _dragPileInfo.height, 32, 0, 0, 0, 0);
        SDL_FillSurfaceRect(
            _p_Dragface, NULL,
            SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Dragface->format), NULL, 0,
                       255, 0));

        SDL_SetSurfaceColorKey(
            _p_Dragface, true,
            SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Dragface->format), NULL, 0,
                       255, 0));
        LPErrInApp err = DrawCardStack(_p_Screen, &dragRegion);
        if (err != NULL) {
            return err;
        }
        dragRegion.InitCardCoords();
        err = DrawCardStack(_p_Dragface, &dragRegion);
        if (err != NULL) {
            return err;
        }

        _oldx = g_pInitDragInfo->x;
        _oldy = g_pInitDragInfo->y;

        updateTextureAsFlipScreen();
        _state = SolitarioGfx::INITDRAG_AFTER;
        TRACE_DEBUG("[STEP 2]InitDrag - end \n");
        return NULL;
    }
    if (_state == SolitarioGfx::INITDRAG_AFTER) {
        updateTextureAsFlipScreen();
        _state = SolitarioGfx::IN_GAME;  // ahead set beacause the callback
                                         // can change the state
        if (_continueFnCb != NULL) {
            TRACE_DEBUG(
                "[INITDRAG_AFTER] InitDrag - run the callback after \n");

            LPErrInApp err = (this->*_continueFnCb)();
            _continueFnCb = NULL;
            if (err != NULL) {
                return err;
            }
        } else {
            TRACE_DEBUG("[INITDRAG_AFTER] InitDrag - nothing to do \n");
        }
    }
    return NULL;
}

void SolitarioGfx::DoDragUpdate(int x, int y) {
    // TRACE_DEBUG(
    //     "DoDragUpdate (x=%d, y=%d) => drag_x=%d, drag_y=%d. old_x=%d,
    //     old_y=%d\n", x, y, _dragPileInfo.x, _dragPileInfo.y, _oldx,
    //     _oldy);

    if (x < _oldx)
        _dragPileInfo.x -= _oldx - x;
    else
        _dragPileInfo.x += x - _oldx;
    if (y < _oldy)
        _dragPileInfo.y -= _oldy - y;
    else
        _dragPileInfo.y += y - _oldy;

    _oldx = x;
    _oldy = y;

    SDL_Rect dest = {0, 0, 0, 0};
    dest.x = _dragPileInfo.x;
    dest.y = _dragPileInfo.y;

    // Do full screen update (On modern hardware, clearing the full screen
    // is cheap and almost always the right tradeoff.)

    SDL_BlitSurface(_p_ScreenBackbufferDrag, NULL, _p_Screen, NULL);
    SDL_BlitSurface(_p_Dragface, NULL, _p_Screen, &dest);

    updateTextureAsFlipScreen();
}

LPCardRegionGfx SolitarioGfx::DoDropEvent() { return DoDropEvent(NULL); }

LPCardRegionGfx SolitarioGfx::DoDropEvent(LPCardRegionGfx pDestRegion) {
    TRACE_DEBUG("[DoDropEvent] -  start, PileInfo.x = %d, PileInfo.y \n",
                _dragPileInfo.x, _dragPileInfo.y);
    SDL_assert(_dragPileInfo.x >= -_p_DeckLoader->GetCardWidth());
    LPCardStackGfx pDestStack;
    LPCardRegionGfx pBestRegion;

    if (pDestRegion != NULL)
        pBestRegion = pDestRegion;
    else
        pBestRegion =
            GetBestStack(_dragPileInfo.x, _dragPileInfo.y, _dragPileInfo.width,
                         _dragPileInfo.height, &_dragStack);
    if (pBestRegion == NULL)
        pBestRegion = _p_selectedCardRegion;  // drop go back to the source, no
                                              // stack found to recive the drag

    pBestRegion->SaveSize();
    pDestStack = pBestRegion->GetCardStack();
    pDestStack->PushStack(&_dragStack);
    pBestRegion->InitCardCoords();

    LPCardGfx pCard = NULL;
    switch (_p_selectedCardRegion->GetDragMode()) {
        case CRD_DRAGTOP:
            pCard = pDestStack->Last();
            break;
        default:
            pCard = pDestStack->Item(pDestStack->Size() - _dragStack.Size());
    }
    _dragStack.Clear();

    if (pCard == NULL) {
        TRACE_DEBUG("DoDropEvent - WARN return on card NULL \n");
        DrawStaticScene();
        _statePrev = _state;
        _state = SolitarioGfx::IN_ZOOM_TERMINATED;
        return pBestRegion;
    }
    if (_dragPileInfo.x == pCard->X() && _dragPileInfo.y == pCard->Y()) {
        TRACE_DEBUG("DoDropEvent -  return on no movement \n");
        DrawStaticScene();
        _statePrev = _state;
        _state = SolitarioGfx::IN_ZOOM_TERMINATED;
        return pBestRegion;  // when no movement
    }

    zoomDropCardStart(&_dragPileInfo.x, &_dragPileInfo.y, pCard,
                      _dragPileInfo.width, _dragPileInfo.height);

    // TRACE_DEBUG("DoDropEvent -  end \n");
    return pBestRegion;
}

typedef struct zoomInfo {
    float i = 0.0;
    int dx;
    int dy;
    float precision = 0.1f;
    int w;
    int h;
    int* pSx = NULL;  // pointer because a card  x,y rect is attached
    int* pSy = NULL;
}* LPzoomInfo;

void zoomInfo_Inc(LPzoomInfo self) {
    if (self) {
        self->i = self->i + self->precision;
    }
}

void zoomInfo_CalcPt(LPzoomInfo self, int& xf, int& yf) {
    if (self) {
        int x0 = *self->pSx;
        int y0 = *self->pSy;
        int x1 = self->dx;
        int y1 = self->dy;
        float t = self->i;
        xf = int(x0 + t * (x1 - x0) + .5);
        yf = int(y0 + t * (y1 - y0) + .5);
    }
}

static LPzoomInfo g_zoomInfo = NULL;

void SolitarioGfx::zoomDropCardStart(int* pSx, int* pSy, LPCardGfx pCard, int w,
                                     int h) {
    TRACE_DEBUG("[zoomDropCardStart] Zoom card - start - x=%d, y=%d \n", *pSx,
                *pSy);
    if (g_zoomInfo != NULL) {
        delete g_zoomInfo;
    }
    g_zoomInfo = new zoomInfo();
    g_zoomInfo->dx = pCard->X();
    g_zoomInfo->dy = pCard->Y();
    g_zoomInfo->pSx = pSx;
    g_zoomInfo->pSy = pSy;

    _statePrev = _state;
    _state = eState::IN_ZOOM;
    TRACE_DEBUG("[zoomDropCardStart] going in state IN_ZOOM from %d \n",
                _statePrev);
}

LPErrInApp SolitarioGfx::zoomDropCardIterate() {
    int px, py;
    zoomInfo_CalcPt(g_zoomInfo, px, py);

    SDL_Rect rcDrag = {0, 0, 0, 0};
    rcDrag.x = px;
    rcDrag.y = py;
    *g_zoomInfo->pSx = px;
    *g_zoomInfo->pSy = py;

    // we are moving the dropped pile to the final destination that could be
    // far away the static scene is fixed into the _p_ScreenBackbufferDrag.
    // To avoid the comet effect, the full display is updated no legacy rect
    // optimization

    //  Do full screen update (On modern hardware, clearing the full screen
    //  is cheap and almost always the right tradeoff.)
    SDL_BlitSurface(_p_ScreenBackbufferDrag, NULL, _p_Screen, NULL);
    SDL_BlitSurface(_p_Dragface, NULL, _p_Screen, &rcDrag);

    updateTextureAsFlipScreen();

    zoomInfo_Inc(g_zoomInfo);
    if (g_zoomInfo->i > 1.0) {
        TRACE_DEBUG("[zoomDropCardIterate] Zoom card - end - x=%d, y=%d \n",
                    *g_zoomInfo->pSx, *g_zoomInfo->pSy);
        _scoreChanged = true;
        DrawStaticScene();
        if (_p_Dragface != NULL) {
            SDL_DestroySurface(_p_Dragface);  // was created in InitDrag,
                                              // time to free it on drop end
            _p_Dragface = NULL;
        }
        _state = SolitarioGfx::IN_ZOOM_TERMINATED;
    }
    return NULL;
}

LPCardRegionGfx SolitarioGfx::FindDropRegion(int id, LPCardGfx pCard) {
    CardStackGfx stack;
    stack.PushCard(pCard);
    return FindDropRegion(id, &stack);
}

LPCardRegionGfx SolitarioGfx::FindDropRegion(int id, LPCardStackGfx pStack) {
    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        if ((vir->RegionTypeId() == id) && vir->CanDrop(pStack))
            return &(*vir);
    }
    return NULL;
}

void SolitarioGfx::DrawStaticScene() {
    // static scene is drawn directly into the screen.
    // Then the screen is copied into the _p_ScreenBackbufferDrag for drag
    // and drop
    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    SDL_FillSurfaceRect(_p_Screen, &clipRect,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    SDL_Rect rctTarget = {0, 0, 0, 0};
    rctTarget.x = (_p_Screen->w - _p_SceneBackground->w) / 2;
    rctTarget.y = (_p_Screen->h - _p_SceneBackground->h) / 2;
    SDL_BlitSurface(_p_SceneBackground, NULL, _p_Screen, &rctTarget);

    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        CardRegionGfx cardRegion = *vir;
        DrawCardStack(&cardRegion);
    }
    _p_BtNewGame->DrawButton(_p_Screen);
    _p_BtQuit->DrawButton(_p_Screen);
    _p_BtToggleSound->DrawButton(_p_Screen);
    _p_BtToggleFullscreen->DrawButton(_p_Screen);
    _scoreChanged = true;
    drawScore();

    SDL_BlitSurface(_p_Screen, NULL, _p_ScreenBackbufferDrag, NULL);

    updateTextureAsFlipScreen();
}

LPErrInApp SolitarioGfx::DrawInitialScene() {
    TRACE("[SolitarioGfx] DrawInitialScene, trigger initial fading\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    if (pGameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
        _p_FadeAction->InstantFade(_p_Screen);
        _state = SolitarioGfx::FIRST_SCENE;
    } else {
        _p_FadeAction->Fade(_p_Screen, _p_Screen, 2, true, _fnUpdateScreen,
                            NULL);
        _state = SolitarioGfx::WAIT_FOR_FADING;
        _stateAfter = SolitarioGfx::FIRST_SCENE;
        return NULL;
    }
    return NULL;
}

LPCardRegionGfx SolitarioGfx::GetBestStack(int x, int y, int w, int h,
                                           LPCardStackGfx pStack) {
    int maxoverlap = 0;
    int percent = 0;
    LPCardRegionGfx pBest = NULL;

    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        if (vir->CanDrop(pStack))
            percent = vir->GetOverlapRatio(x, y, w,
                                           h);  // stack candidate for dropping
        else
            percent = 0;

        if (percent > maxoverlap) {
            maxoverlap = percent;
            pBest = &(*vir);
        }
    }
    return pBest;
}

LPErrInApp SolitarioGfx::DrawCard(int x, int y, int nCdIndex) {
    return DrawCard(x, y, nCdIndex, _p_Screen, 0.0);
}

LPErrInApp SolitarioGfx::DrawCard(int x, int y, int nCdIndex, SDL_Surface* s,
                                  float scaleFactor) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Draw a card on NULL surface. This is wrong");
    }
    return DrawCardPac(x, y, nCdIndex, s, scaleFactor);
}

LPErrInApp SolitarioGfx::DrawCardPac(int x, int y, int nCdIndex, SDL_Surface* s,
                                     float scaleFactor) {
    if (nCdIndex < 0 || nCdIndex >= _deckType.GetNumCards())
        return ERR_UTIL::ErrorCreate("DrawCardPac %d index out of range",
                                     nCdIndex);

    int suitIx = nCdIndex / _deckType.GetNumCardInSuit();
    int cardIx = nCdIndex % _deckType.GetNumCardInSuit();
    // TRACE_DEBUG("Suit %d, card: %d\n", suitIx, cardIx);

    SDL_Rect srcCard;
    srcCard.x = suitIx * _p_DeckLoader->GetCardWidth();
    srcCard.y = cardIx * _p_DeckLoader->GetCardHeight();
    srcCard.w = _p_DeckLoader->GetCardWidth();
    srcCard.h = _p_DeckLoader->GetCardHeight();
    SDL_Surface* pDeck = _p_DeckLoader->GetDeckSurface();        
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    if (scaleFactor != 0.0f) {
        dest.w = (int)(srcCard.w * scaleFactor);
        dest.h = (int)(srcCard.h * scaleFactor);
        if (!SDL_BlitSurfaceScaled(pDeck, &srcCard, s, &dest,
                                   SDL_SCALEMODE_NEAREST)) {
            return ERR_UTIL::ErrorCreate(
                "SDL_BlitSurfaceScaled in DrawCardPac error: %s\n",
                SDL_GetError());
        }
    } else if (!SDL_BlitSurface(pDeck, &srcCard, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardPac error: %s\n", SDL_GetError());
    }
    return NULL;
}

LPErrInApp SolitarioGfx::DrawCard(LPCardGfx pCard, SDL_Surface* s,
                                  float scaleFactor) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate("Error in draw card, surface is NULL\n");
    }
    // TRACE("Draw card ix = %d, suit = %s, rank %d, x,y %d,%d",
    return DrawCardPac(pCard, s, scaleFactor);
}

LPErrInApp SolitarioGfx::DrawCardPac(LPCardGfx pCard, SDL_Surface* s,
                                     float scaleFactor) {
    pCard->SetDeckSurface(_p_DeckLoader->GetDeckSurface());

    return pCard->DrawCardPac(s);
}

LPErrInApp SolitarioGfx::DrawCardBack(int x, int y) {
    return DrawCardBack(x, y, _p_Screen, 0.0);
}

LPErrInApp SolitarioGfx::DrawCardBack(int x, int y, SDL_Surface* s,
                                      float scaleFactor) {
    TRACE_DEBUG("DrawCardBack x=%d, y=%d\n", x, y);
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Error in DrawCardBack, surface is NULL\n");
    }

    return DrawCardBackPac(x, y, s, scaleFactor);
}

LPErrInApp SolitarioGfx::DrawCardBackPac(int x, int y, SDL_Surface* s,
                                         float scaleFactor) {
    SDL_Rect dest, srcBack;
    dest.x = x;
    dest.y = y;

    // Card back is index 0 in the symbols pac
    srcBack.x = 0;
    srcBack.y = 0;
    srcBack.w = _p_DeckLoader->GetSymbolWidth();
    srcBack.h = _p_DeckLoader->GetSymbolHeight();

    if (scaleFactor != 0.0f) {
        dest.w = (int)(_p_DeckLoader->GetCardWidth() * scaleFactor);
        dest.h = (int)(_p_DeckLoader->GetCardHeight() * scaleFactor);
        if (!SDL_BlitSurfaceScaled(_p_DeckLoader->GetSymbolSurface(), &srcBack, s, &dest,
                                   SDL_SCALEMODE_NEAREST)) {
            return ERR_UTIL::ErrorCreate(
                "SDL_BlitSurfaceScaled in DrawCardBackPac error: %s\n",
                SDL_GetError());
        }
    } else if (!SDL_BlitSurface(_p_DeckLoader->GetSymbolSurface(), &srcBack, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardBackPac error: %s\n", SDL_GetError());
    }

    return NULL;
}

LPErrInApp SolitarioGfx::DrawSymbol(int x, int y, int nSymbol) {
    return DrawSymbol(x, y, nSymbol, _p_Screen, 0.0);
}

LPErrInApp SolitarioGfx::DrawSymbol(int x, int y, int nSymbol, SDL_Surface* s,
                                    float scaleFactor) {
    if (nSymbol < 1) {
        return ERR_UTIL::ErrorCreate("Symbol index %d out of range", nSymbol);
    }
    if (nSymbol > 3)
        nSymbol = 3;

    return DrawSymbolPac(x, y, nSymbol, s, scaleFactor);
}

LPErrInApp SolitarioGfx::DrawSymbolPac(int x, int y, int nSymbol,
                                       SDL_Surface* s, float scaleFactor) {
    if (nSymbol < 0 || nSymbol >= 4)
        return ERR_UTIL::ErrorCreate("DrawSymbol %d index out of range",
                                     nSymbol);

    SDL_Rect srcSymb;
    srcSymb.x = nSymbol * _p_DeckLoader->GetSymbolWidth();
    srcSymb.y = 0;
    srcSymb.w = _p_DeckLoader->GetSymbolWidth();
    srcSymb.h = _p_DeckLoader->GetSymbolHeight();

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    if (scaleFactor != 0.0) {
        dest.w = (int)(srcSymb.w * scaleFactor);
        dest.h = (int)(srcSymb.h * scaleFactor);
        if (!SDL_BlitSurfaceScaled(_p_DeckLoader->GetSymbolSurface(), &srcSymb, s, &dest,
                                   SDL_SCALEMODE_NEAREST)) {
            return ERR_UTIL::ErrorCreate(
                "SDL_BlitSurfaceScaled in DrawSymbol error: %s\n",
                SDL_GetError());
        }
    } else if (!SDL_BlitSurface(_p_DeckLoader->GetSymbolSurface(), &srcSymb, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawSymbol error: %s\n", SDL_GetError());
    }
    return NULL;
}

typedef struct victoryInfo {
    int rotation = 0;
    int id = 0;
    int x = 0;
    int y = 0;
    int xspeed = 0;
    int yspeed = 0;
    int gravity = 1;
    float bounce = 0.8f;
    int max_y = 0;
}* LPvictoryInfo;
static LPvictoryInfo g_pVict = NULL;

void SolitarioGfx::clearAnimation() {
    if (g_pVict != NULL) {
        delete g_pVict;
        g_pVict = NULL;
    }
}

bool SolitarioGfx::isInVictoryState() {
    return (_state == SolitarioGfx::IN_CARD_VICTORY ||
            _state == SolitarioGfx::START_VICTORY ||
            _state == SolitarioGfx::NEW_CARD_VICTORY);
}

LPErrInApp SolitarioGfx::VictoryAnimationIterate() {
    if (!isInVictoryState()) {
        return ERR_UTIL::ErrorCreate(
            "[VictoryAnimation] called in worng state");
    }

    if (_state == SolitarioGfx::START_VICTORY) {
        TRACE("Victory animation - Start \n");
        if (g_pVict != NULL) {
            delete g_pVict;
        }
        g_pVict = new victoryInfo();
        g_pVict->max_y = _p_Screen->h;
        DrawStaticScene();
        _state = SolitarioGfx::NEW_CARD_VICTORY;
        return NULL;
    }

    if (_state == SolitarioGfx::NEW_CARD_VICTORY) {
        if (g_pVict == NULL) {
            _state = SolitarioGfx::IN_GAME;
            return NULL;
        }
        g_pVict->rotation = rand() % 2;
        g_pVict->id = rand() % _deckType.GetNumCards();
        g_pVict->x = rand() % _p_Screen->w;
        g_pVict->y = rand() % _p_Screen->h / 2;

        if (g_pVict->rotation)
            g_pVict->xspeed = -4;
        else
            g_pVict->xspeed = 4;

        g_pVict->yspeed = 0;
        _state = SolitarioGfx::IN_CARD_VICTORY;
    }
    if (_state == SolitarioGfx::IN_CARD_VICTORY) {
        if (g_pVict == NULL) {
            _state = SolitarioGfx::IN_GAME;
            return NULL;
        }
        g_pVict->yspeed = g_pVict->yspeed + g_pVict->gravity;
        g_pVict->x += g_pVict->xspeed;
        g_pVict->y += g_pVict->yspeed;

        if (g_pVict->y + _p_DeckLoader->GetCardHeight() > g_pVict->max_y) {
            g_pVict->y = g_pVict->max_y - _p_DeckLoader->GetCardHeight();
            g_pVict->yspeed = int(-g_pVict->yspeed * g_pVict->bounce);
        }
        LPErrInApp err = DrawCard(g_pVict->x, g_pVict->y, g_pVict->id,
                                  _p_Screen, _scaleFactor);
        if (err != NULL) {
            return err;
        }
        updateTextureAsFlipScreen();

        if ((g_pVict->x + _p_DeckLoader->GetCardWidth() <= 0) || (g_pVict->x >= _p_Screen->w)) {
            _state = SolitarioGfx::NEW_CARD_VICTORY;
        }
    }
    return NULL;
}

LPErrInApp SolitarioGfx::newGame() {
    TRACE("[SolitarioGfx] newGame\n");
    LPErrInApp err;
    clearAnimation();
    SetSymbol(DeckPile_Ix, CRD_OSYMBOL);
    CleanUpRegion();
    _p_currentTime->Reset();
    clearScore();

    err = NewDeck(DeckPile_Ix);
    if (err != NULL) {
        return err;
    }
    Shuffle(DeckPile_Ix);

    // deal
    int i;
    for (i = eRegionIx::Found_Ix1; i <= eRegionIx::Found_Ix7; i++) {
        LPCardStackGfx pStack = PopStackFromRegion(eRegionIx::DeckPile_Ix, i);
        PushStackInRegion(i, pStack);
        delete pStack;
    }

    InitAllCoords(true);

    for (i = Found_Ix1; i <= Found_Ix7; i++) {
        SetCardFaceUp(i, true, RegionSize(i) - 1);
    }
    return NULL;
}

// Remember Finger events are parallel with mouse events
LPErrInApp SolitarioGfx::handleGameLoopFingerDownEvent(SDL_Event* pEvent) {
    TRACE_DEBUG("handleGameLoopFingerDownEvent (TAP only) \n");
    Uint64 now_time = SDL_GetTicks();
    _p_BtQuit->FingerDown(pEvent);
    _p_BtNewGame->FingerDown(pEvent);
    _p_BtToggleSound->FingerDown(pEvent);
    _p_BtToggleFullscreen->FingerDown(pEvent);

    SDL_Point pt;
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    pGameSettings->GetTouchPoint(pEvent->tfinger, &pt);
    LPErrInApp err;
    if (_lastUpTimestamp + _doubleTapWait > now_time) {
        err = doubleTapOrRightClick(pt);
        if (err != NULL) {
            return NULL;
        }
    } else {
        err = singleTapOrLeftClick(pt);
        if (err != NULL) {
            return NULL;
        }
    }
    _lastUpTimestamp = SDL_GetTicks();
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopFingerUpEvent(SDL_Event* pEvent) {
    TRACE_DEBUG("handleGameLoopFingerUpEvent\n");
    _isInitDrag = false;
    LPErrInApp err = endOfDragAndCheckForVictory();
    if (err != NULL) {
        return NULL;
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopFingerMotion(SDL_Event* pEvent) {
    if (_startdrag) {
        SDL_Point pt;
        LPGameSettings pGameSettings = GameSettings::GetSettings();
        pGameSettings->GetTouchPoint(pEvent->tfinger, &pt);
        DoDragUpdate(pt.x, pt.y);
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopMouseDownEvent(
    SDL_Event* pEvent, const SDL_Point& targetPos) {
    if (_startdrag) {
        TRACE_DEBUG(
            "Ignore Mousedown because is drag ongoing. Wait for drag end "
            "with mouse up \n");
        return NULL;
    }
    if (pEvent->button.button == SDL_BUTTON_LEFT) {
        Uint64 now_time = SDL_GetTicks();
        _ptLast.x = targetPos.x;
        _ptLast.y = targetPos.y;
        if (_lastUpTimestamp + _doubleTapWait > now_time) {
            _state = SolitarioGfx::IN_DOUBLE_TAPCLICK;
        } else {
            _state = SolitarioGfx::IN_SINGLE_TAPCLICK;
        }
        _lastUpTimestamp = SDL_GetTicks();
    } else if (pEvent->button.button == SDL_BUTTON_RIGHT) {
        _ptLast.x = targetPos.x;
        _ptLast.y = targetPos.y;
        _state = SolitarioGfx::IN_DOUBLE_TAPCLICK;
    }
    return NULL;
}

LPErrInApp SolitarioGfx::doubleTapOrRightClick(SDL_Point& pt) {
    TRACE_DEBUG("doubleTapOrRightClick recognized\n");
    LPErrInApp err;
    bool isInitDrag = false;

    CardRegionGfx* pCardRegSrc = SelectRegionOnPoint(pt.x, pt.y);
    if (pCardRegSrc == NULL) {
        return NULL;
    }
    LPCardGfx pCard = pCardRegSrc->GetCard(pCardRegSrc->Size() - 1);
    if (pCard == NULL) {
        return NULL;
    }
    if (_p_DropRegionForDrag != NULL) {
        TRACE_DEBUG(
            "[doubleTapOrRightClick] WARN Drag/Drop already ongoing "
            "(DropRegionForDrag) \n");
        return NULL;
    }
    if (_p_CardStackForDrag != NULL) {
        TRACE_DEBUG(
            "[doubleTapOrRightClick] WARN Drag/Drop already ongoing "
            "(CardStackForDrag) \n");
        return NULL;
    }

    if (((pCardRegSrc->RegionTypeId() == RegionType::RT_TABLEAU) ||
         (pCardRegSrc->RegionTypeId() == RegionType::RT_DECKSTOCK_FACEUP)) &&
        pCard->IsFaceUp() && pCardRegSrc->PtOnTop(pt.x, pt.y)) {
        _p_DropRegionForDrag =
            FindDropRegion(RegionType::RT_ACE_FOUNDATION, pCard);
        if (_p_DropRegionForDrag == NULL) {
            return NULL;
        }
        _p_CardStackForDrag = pCardRegSrc->PopStack(1);
        if (_p_CardStackForDrag == NULL) {
            return NULL;
        }
        err = InitDrag(_p_CardStackForDrag, -1, -1, isInitDrag, pCardRegSrc);
        if (err != NULL) {
            return err;
        }
        _continueFnCb = &SolitarioGfx::InitDragAfterFromDoubleTap;
        TRACE_DEBUG("[doubleTapOrRightClick] is in process\n");
        return NULL;
    }
    if (pCardRegSrc->RegionTypeId() == RegionType::RT_DECKSTOCK) {
        TRACE_DEBUG(
            "[doubleTapOrRightClick] on RT_DECKSTOCK is a single click\n");
        err = clickOnDeckStock(pCardRegSrc);
        if (err != NULL) {
            return err;
        }
    }

    return NULL;
}

LPErrInApp SolitarioGfx::InitDragAfterFromDoubleTap() {
    TRACE_DEBUG("InitDragAfterFromDoubleTap \n");

    DoDropEvent(_p_DropRegionForDrag);
    _continueLamdaCb = [this]() -> LPErrInApp {
        updateScoreOnAce(_p_DropRegionForDrag->Size(),
                         _p_DropRegionForDrag->GetSavedSize());
        if (_p_CardStackForDrag != NULL) {
            delete _p_CardStackForDrag;
            _p_CardStackForDrag = NULL;
        }
        _p_DropRegionForDrag = NULL;
        TRACE_DEBUG("InitDragAfterFromDoubleTap - drop end \n");
        return NULL;
    };
    return NULL;
}

LPErrInApp SolitarioGfx::InitDragAfterFromSingleTapA() {
    TRACE_DEBUG("InitDragAfterFromSingleTapA \n");

    if (_isInitDrag) {
        TRACE_DEBUG("dragging intialize ok - now start drag \n");
        _startdrag = true;
        SDL_HideCursor();
        SDL_SetWindowMouseGrab(_p_Window, true);
        _isInitDrag = false;
    } else {
        TRACE_DEBUG(
            "!![InitDragAfterFromSingleTapA]!! dragging is over, force "
            "drop\n");
        DoDropEvent();
    }
    return NULL;
}

LPErrInApp SolitarioGfx::InitDragAfterFromSingleTapB() {
    TRACE_DEBUG("InitDragAfterFromSingleTapB \n");

    DoDropEvent(GetRegion(eRegionIx::DeckPile_Ix));
    _continueLamdaCb = [this]() -> LPErrInApp {
        Reverse(eRegionIx::DeckPile_Ix);
        InitCardCoords(eRegionIx::DeckPile_Ix);
        if (_p_CardStackForDrag != NULL) {
            delete _p_CardStackForDrag;
            _p_CardStackForDrag = NULL;
        }
        updateBadScoreRedial();
        TRACE_DEBUG("InitDragAfterFromSingleTapB - drop end \n");
        return NULL;
    };

    return NULL;
}

LPErrInApp SolitarioGfx::InitDragAfterFromSingleTapC() {
    TRACE_DEBUG("InitDragAfterFromSingleTapC \n");

    DoDropEvent(GetRegion(eRegionIx::DeckFaceUp));
    _continueLamdaCb = [this]() -> LPErrInApp {
        if (_p_CardStackForDrag != NULL) {
            delete _p_CardStackForDrag;
            _p_CardStackForDrag = NULL;
        }
        TRACE_DEBUG("InitDragAfterFromSingleTapC - drop end \n");
        return NULL;
    };

    return NULL;
}

LPErrInApp SolitarioGfx::singleTapOrLeftClick(SDL_Point& pt) {
    TRACE_DEBUG("[singleTapOrLeftClick] in state %d\n", _state);
    LPErrInApp err;
    CardRegionGfx* pCardRegSrc;
    bool isInitDrag = false;

    pCardRegSrc = SelectRegionOnPoint(pt.x, pt.y);
    if (pCardRegSrc == NULL) {
        return NULL;
    }

    if ((pCardRegSrc->RegionTypeId() == RegionType::RT_TABLEAU) &&
        pCardRegSrc->PtOnTop(pt.x, pt.y)) {
        int id = pCardRegSrc->Size() - 1;
        if (!pCardRegSrc->IsCardFaceUp(id)) {
            pCardRegSrc->SetCardFaceUp(true, id);
            updateScoreOnTurnOverFaceDown();
        }
    }

    if ((pCardRegSrc->RegionTypeId() == RegionType::RT_TABLEAU) ||
        (pCardRegSrc->RegionTypeId() == RegionType::RT_DECKSTOCK_FACEUP) ||
        (pCardRegSrc->RegionTypeId() == RegionType::RT_ACE_FOUNDATION)) {
        // clicked on region that can do dragging
        err = InitDrag(pt.x, pt.y, isInitDrag, pCardRegSrc);
        if (err != NULL) {
            return err;
        }
        _isInitDrag = isInitDrag;
        _continueFnCb = &SolitarioGfx::InitDragAfterFromSingleTapA;
    } else if (pCardRegSrc->RegionTypeId() == RegionType::RT_DECKSTOCK) {
        if (_p_CardStackForDrag == NULL) {
            err = clickOnDeckStock(pCardRegSrc);
            if (err != NULL) {
                return err;
            }

        } else {
            TRACE_DEBUG(
                "[singleTapOrLeftClick] WARN Tap on RT_DECKSTOCK ignored "
                "because drag is ongoing \n");
        }
    }
    return NULL;
}

LPErrInApp SolitarioGfx::clickOnDeckStock(CardRegionGfx* pCardRegSrc) {
    bool isInitDrag = false;
    LPErrInApp err = NULL;
    if (pCardRegSrc->IsEmpty() && !IsRegionEmpty(eRegionIx::DeckFaceUp)) {
        // from deckfaceup back to deckpile: drag and drop to deckpile
        // (redial)
        _p_CardStackForDrag = PopStackFromRegion(
            eRegionIx::DeckFaceUp, RegionSize(eRegionIx::DeckFaceUp));
        _p_CardStackForDrag->SetCardsFaceUp(false);
        err = InitDrag(_p_CardStackForDrag, -1, -1, isInitDrag, pCardRegSrc);
        if (err != NULL) {
            return err;
        }
        _continueFnCb = &SolitarioGfx::InitDragAfterFromSingleTapB;
    } else if (!pCardRegSrc->IsEmpty()) {
        // the next card goes to the deck face up region: drag and drop
        // to deck face up
        _p_CardStackForDrag = PopStackFromRegion(eRegionIx::DeckPile_Ix, 1);
        _p_CardStackForDrag->SetCardsFaceUp(true);
        err = InitDrag(_p_CardStackForDrag, -1, -1, isInitDrag, pCardRegSrc);
        if (err != NULL) {
            return err;
        }
        _continueFnCb = &SolitarioGfx::InitDragAfterFromSingleTapC;
    } else {
        TRACE("No more card on the pile deck\n");
    }
    return NULL;
}

void SolitarioGfx::handleGameLoopMouseMoveEvent(SDL_Event* pEvent,
                                                const SDL_Point& targetPos) {
    if (pEvent->motion.state == SDL_BUTTON_MASK(1) && _startdrag) {
        // TRACE_DEBUG("handleGameLoopMouseMoveEvent - DoDrag trigger \n");
        DoDragUpdate(targetPos.x, targetPos.y);
    }
    bool statusChanged = _p_BtNewGame->MouseMove(pEvent, targetPos);
    statusChanged = statusChanged || _p_BtQuit->MouseMove(pEvent, targetPos) ||
                    _p_BtToggleSound->MouseMove(pEvent, targetPos) ||
                    _p_BtToggleFullscreen->MouseMove(pEvent, targetPos);
    if (statusChanged) {
        // TRACE_DEBUG("handleGameLoopMouseMoveEvent - status changed \n");
        DrawStaticScene();
    }
}

LPErrInApp SolitarioGfx::handleGameLoopMouseUpEvent(
    SDL_Event* pEvent, const SDL_Point& targetPos) {
    TRACE_DEBUG("handleGameLoopMouseUpEvent \n");
    _isInitDrag = false;
    _p_BtQuit->MouseUp(pEvent, targetPos);
    _p_BtNewGame->MouseUp(pEvent, targetPos);
    _p_BtToggleSound->MouseUp(pEvent, targetPos);
    _p_BtToggleFullscreen->MouseUp(pEvent, targetPos);
    return endOfDragAndCheckForVictory();
}

LPErrInApp SolitarioGfx::endOfDragAndCheckForVictory() {
    if (_startdrag) {
        TRACE_DEBUG("endOfDrag -> end \n");
        _startdrag = false;
        LPCardRegionGfx pDestReg = DoDropEvent();
        _continueLamdaCb = [this, pDestReg]() -> LPErrInApp {
            SDL_SetWindowMouseGrab(_p_Window, false);

            if (pDestReg->RegionTypeId() == RegionType::RT_ACE_FOUNDATION) {
                updateScoreOnAce(pDestReg->Size(), pDestReg->GetSavedSize());
            } else if (_dragPileInfo.pSrcRegion->RegionTypeId() ==
                           RegionType::RT_DECKSTOCK_FACEUP &&
                       pDestReg->RegionTypeId() == RegionType::RT_TABLEAU) {
                updateScoreMoveDeckToTableau();
            } else if (_dragPileInfo.pSrcRegion->RegionTypeId() ==
                           RegionType::RT_ACE_FOUNDATION &&
                       pDestReg->RegionTypeId() == RegionType::RT_TABLEAU) {
                updateBadScoreAceToTableu();
            }
            LPErrInApp err = checkForVictoryEvent();
            if (err != NULL) {
                return err;
            }
            if (_p_CardStackForDrag != NULL) {
                delete _p_CardStackForDrag;
                _p_CardStackForDrag = NULL;
            }
            TRACE_DEBUG("endOfDragAndCheckForVictory - drop end \n");
            return NULL;
        };
    }
    SDL_ShowCursor();
    return checkForVictoryEvent();
}

LPErrInApp SolitarioGfx::checkForVictoryEvent() {
    if (IsRegionEmpty(DeckPile_Ix) && IsRegionEmpty(DeckFaceUp)) {
        SetSymbol(DeckPile_Ix, CRD_XSYMBOL);
        DrawStaticScene();
    }
    // victory
    LPErrInApp err;
    int numCardOnSUit = _deckType.GetNumCardInSuit();
    if ((RegionSize(Ace_Ix1) == numCardOnSUit) &&
        (RegionSize(Ace_Ix2) == numCardOnSUit) &&
        (RegionSize(Ace_Ix3) == numCardOnSUit) &&
        (RegionSize(Ace_Ix4) == numCardOnSUit)) {
        TRACE_DEBUG("Victory \n");
        _p_currentTime->StopTimer();
        bonusScore();
        DrawStaticScene();
        err = (_fnUpdateHighScore.tc)
                  ->SaveScore(_fnUpdateHighScore.self, _scoreGame,
                              _deckType.GetNumCards());
        if (err != NULL) {
            return err;
        }
        _state = SolitarioGfx::START_VICTORY;
    }

    return NULL;
}

LPErrInApp SolitarioGfx::HandleEvent(SDL_Event* pEvent,
                                     const SDL_Point& targetPos) {
    LPErrInApp err = NULL;
    if (isInVictoryState()) {
        switch (pEvent->type) {
            case SDL_EVENT_QUIT:
                clearAnimation();
                _state = eState::SHOW_SCORE;
                return NULL;
            case SDL_EVENT_KEY_DOWN:
                if (pEvent->key.key == SDLK_ESCAPE ||
                    pEvent->key.key == SDLK_SPACE ||
                    pEvent->key.key == SDLK_RETURN) {
                    clearAnimation();
                    _state = eState::SHOW_SCORE;
                    return NULL;
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                clearAnimation();
                _state = eState::SHOW_SCORE;
                return NULL;
            case SDL_EVENT_FINGER_DOWN:
                clearAnimation();
                _state = eState::SHOW_SCORE;
                return NULL;
        }
        return NULL;
    }

    if (_state == eState::IN_MSGBOX) {
        _p_MsgBox->HandleEvent(pEvent, targetPos);
        return NULL;
    }
    if (_state == eState::IN_ZOOM || _state == eState::IN_ZOOM_TERMINATED) {
        return NULL;
    }
    if (_state == eState::FADING_OUT || _state == eState::WAIT_FOR_FADING) {
        if (pEvent->key.key == SDLK_ESCAPE) {
            TRACE_DEBUG("[SolitarioGfx - event] escape\n");
            _state = eState::TERMINATED;
            return NULL;
        }
        return NULL;
    }
    switch (pEvent->type) {
        case SDL_EVENT_QUIT:
            TRACE_DEBUG("[SolitarioGfx - event] quit\n");
            _state = eState::FADING_OUT;
            return NULL;

        case SDL_EVENT_KEY_DOWN:
            if (pEvent->key.key == SDLK_ESCAPE) {
                TRACE_DEBUG("[SolitarioGfx - event] escape\n");
                Uint32 flags = SDL_GetWindowFlags(_p_Window);
                STRING strTextBt;
                if (flags & SDL_WINDOW_FULLSCREEN) {
                    BtToggleFullscreenClick();
                } else {
                    _state = eState::TERMINATED;
                }
                return NULL;
            }
            if (pEvent->key.key == SDLK_A) {
                TRACE_DEBUG("[SolitarioGfx - event] animation victory\n");
                _state = eState::START_VICTORY;
            }
            if (pEvent->key.key == SDLK_N) {
                TRACE_DEBUG("[SolitarioGfx - event] ask for a new game\n");
                _state = eState::ASK_FOR_NEW_GAME;
            }
            if (pEvent->key.key == SDLK_F) {
                TRACE_DEBUG("[SolitarioGfx - event] toggle fullscreen\n");
                BtToggleFullscreenClick();
            }
            break;
#if HASTOUCH
        case SDL_EVENT_FINGER_DOWN:
            TRACE_DEBUG("Event SDL_EVENT_FINGER_DOWN \n");
            err = handleGameLoopFingerDownEvent(pEvent);
            if (err != NULL)
                return err;
            break;
        case SDL_EVENT_FINGER_UP:
            TRACE_DEBUG("Event SDL_EVENT_FINGER_UP \n");
            err = handleGameLoopFingerUpEvent(pEvent);
            if (err != NULL)
                return err;
            break;
        case SDL_EVENT_FINGER_MOTION:
            err = handleGameLoopFingerMotion(pEvent);
            if (err != NULL)
                return err;
            break;
#endif
#if HASMOUSE
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (_state == eState::INITDRAG_STEP1 ||
                _state == eState::INITDRAG_STEP2 ||
                _state == eState::INITDRAG_AFTER) {
                return NULL;
            }
            TRACE_DEBUG(
                "Event SDL_EVENT_MOUSE_BUTTON_DOWN - start (state %d, "
                "_startdrag = %d)\n",
                _state, _startdrag);
            err = handleGameLoopMouseDownEvent(pEvent, targetPos);
            // TRACE_DEBUG("Event SDL_EVENT_MOUSE_BUTTON_DOWN - end
            // \n");
            if (err != NULL)
                return err;
            break;

        case SDL_EVENT_MOUSE_MOTION:
            handleGameLoopMouseMoveEvent(pEvent, targetPos);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            TRACE_DEBUG("Event SDL_EVENT_MOUSE_BUTTON_UP \n");
            err = handleGameLoopMouseUpEvent(pEvent, targetPos);
            if (err != NULL)
                return err;
            break;
#endif
    }
    return NULL;
}

LPErrInApp SolitarioGfx::HandleIterate(bool& done) {
    LPErrInApp err = NULL;
    if (_state == SolitarioGfx::WAIT_FOR_FADING) {
        if (_p_FadeAction->IsInProgress()) {
            _p_FadeAction->Iterate();
            return NULL;
        }
        TRACE_DEBUG(
            "[SolitarioGfx - Iterate] - fade end, from %d to next state %d "
            "\n",
            _state, _stateAfter);
        if (_state == _stateAfter) {
            return ERR_UTIL::ErrorCreate(
                "Next state could not be WAIT_FOR_FADING\n");
        }
        _state = _stateAfter;
    }

    if (_state == SolitarioGfx::READY_TO_START) {
        TRACE_DEBUG("[SolitarioGfx - Iterate] ready to start\n");
        DrawInitialScene();
        return NULL;
    }

    if (_state == SolitarioGfx::FIRST_SCENE) {
        TRACE_DEBUG("[SolitarioGfx - Iterate] first scene\n");
        DrawStaticScene();
        _state = SolitarioGfx::IN_GAME;
        return NULL;
    }

    if (_state == SolitarioGfx::DO_NEWGAME) {
        TRACE_DEBUG("[SolitarioGfx - Iterate] state DO_NEWGAME \n");
        newGame();
        DrawStaticScene();
        _state = SolitarioGfx::IN_GAME;
    }

    if (_state == SolitarioGfx::INITDRAG_STEP1 ||
        _state == SolitarioGfx::INITDRAG_STEP2 ||
        _state == SolitarioGfx::INITDRAG_AFTER) {
        // TRACE_DEBUG("[SolitarioGfx - Iterate] INITDRAG_%d \n", _state);
        InitDragContinueIterate();
        return NULL;
    }

    if (_state == SolitarioGfx::IN_ZOOM) {
        return zoomDropCardIterate();
    }
    if (_state == SolitarioGfx::IN_ZOOM_TERMINATED) {
        _state = _statePrev;
        if (_continueLamdaCb != NULL) {
            TRACE_DEBUG(
                "[SolitarioGfx - Iterate] run the callback after drop zoom "
                "(next state %d) \n",
                _state);
            LPErrInApp err = _continueLamdaCb();
            _continueLamdaCb = NULL;
            if (err != NULL) {
                return err;
            }
            _p_DropRegionForDrag = NULL;
        } else {
            TRACE_DEBUG(
                "[SolitarioGfx - Iterate] nothing to do after the zoom "
                "(next "
                "state %d) \n",
                _state);
        }
        return NULL;
    }

    if (_state == SolitarioGfx::IN_GAME) {
        if (_statePrev != _state) {
            DrawStaticScene();
            _statePrev = _state;
        }
        updateBadScoreScoreOnTime();
        // write direct into the screen because it could be that a
        // is in action and the screen for a back buffer is dirty
        err = drawScore();
        if (err != NULL)
            return err;
    }

    if (_state == SolitarioGfx::IN_DOUBLE_TAPCLICK) {
        TRACE_DEBUG("[SolitarioGfx - Iterate] double tap click state \n");
        _state = SolitarioGfx::IN_GAME;
        doubleTapOrRightClick(_ptLast);
    } else {
        if (_state == SolitarioGfx::IN_SINGLE_TAPCLICK) {
            _state = SolitarioGfx::IN_GAME;
            singleTapOrLeftClick(_ptLast);
        }
    }
    if (_state == SolitarioGfx::ASK_FOR_NEW_GAME) {
        _state = SolitarioGfx::IN_GAME;
        BtNewGameClick();
        return NULL;
    }

    if (_state == SolitarioGfx::FADING_OUT) {
        TRACE_DEBUG("[SolitarioGfx - Iterate] - enter in state FADING_OUT \n");
        _p_FadeAction->Fade(_p_Screen, _p_Screen, 1, true, _fnUpdateScreen,
                            NULL);
        if (_p_MusicManager->IsPlayingMusic()) {
            _p_MusicManager->StopMusic(300);
        }
        _state = SolitarioGfx::WAIT_FOR_FADING;
        _stateAfter = SolitarioGfx::TERMINATED;
        return NULL;
    }

    if (isInVictoryState()) {
        return VictoryAnimationIterate();
    }

    if (_state == SHOW_SCORE) {
        TRACE_DEBUG("Show score state \n");
        char buff[1024];
        LPGameSettings pGameSettings = GameSettings::GetSettings();
        LPLanguages pLanguages = pGameSettings->GetLanguageMan();
        snprintf(buff, 1024, pLanguages->GetCStringId(Languages::FINAL_SCORE),
                 _scoreGame);
        _state = SolitarioGfx::IN_MSGBOX;
        _stateAfter = SolitarioGfx::DO_NEWGAME;
        showOkMsgBox(buff);
    }

    if (_state == SolitarioGfx::TERMINATED) {
        TRACE_DEBUG("[SolitarioGfx - Iterate] state Terminated \n");
        if (_p_MusicManager->IsPlayingMusic()) {
            _p_MusicManager->StopMusic(0);
        }
        done = true;
    }

    if (_state == SolitarioGfx::IN_MSGBOX) {
        bool msgBoxDone = false;
        _p_MsgBox->HandleIterate(msgBoxDone);

        if (msgBoxDone) {
            if (_p_MsgBox->GetType() ==
                MesgBoxGfx::eMSGBOX_TYPE::TY_MB_YES_NO) {
                TRACE_DEBUG("User click result %d \n", _p_MsgBox->GetResult());
                if (_p_MsgBox->GetResult() ==
                    MesgBoxGfx::eMSGBOX_RES::RES_YES) {
                    TRACE_DEBUG("User choose YES \n");
                    _state = _stateAfterYes;
                } else {
                    _state = _stateAfterNo;
                    TRACE_DEBUG("User choose NO \n");
                }
            } else {
                TRACE_DEBUG("Message box close \n");
                _state = _stateAfter;
            }
            TRACE_DEBUG("Next state is %d \n", _state);
            _statePrev = SolitarioGfx::IN_MSGBOX;
        }
    }
    return NULL;
}

LPErrInApp SolitarioGfx::Show() {
    TRACE_DEBUG("[SolitarioGfx] Show, card size: width %d, height %d\n",
                _p_DeckLoader->GetCardWidth(), _p_DeckLoader->GetCardHeight());

    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();

    _cardRegionList.clear();

    _p_MusicManager->PlayMusic(MusicManager::MUSIC_PLAY_SND,
                               MusicManager::eLoopType::LOOP_ON);
    // button Quit
    STRING strTextBt;
    strTextBt = pLanguages->GetStringId(Languages::ID_EXIT);
    _p_BtQuit->SetButtonText(strTextBt.c_str());
    _p_BtQuit->SetVisibleState(ButtonGfx::VISIBLE);
    // button New Game
    strTextBt = pLanguages->GetStringId(Languages::ID_NEWGAME);
    _p_BtNewGame->SetButtonText(strTextBt.c_str());
    _p_BtNewGame->SetVisibleState(ButtonGfx::VISIBLE);
    // button Toggle Sound
    if (_p_MusicManager->IsMusicEnabled()) {
        if (_p_MusicManager->IsPlayingMusic()) {
            strTextBt = "🔊";
        } else {
            strTextBt = "🔇";
        }
        _p_BtToggleSound->SetButtonText(strTextBt.c_str());
        _p_BtToggleSound->SetVisibleState(ButtonGfx::VISIBLE);
    }

    // button Toggle Fullscreen
    Uint32 flags = SDL_GetWindowFlags(_p_Window);
    if (flags & SDL_WINDOW_FULLSCREEN) {
        strTextBt = "✕";
    } else {
        strTextBt = "🗖";
    }
    _p_BtToggleFullscreen->SetButtonText(strTextBt.c_str());
    _p_BtToggleFullscreen->SetVisibleState(ButtonGfx::VISIBLE);

    // index 0 (deck with face down)
    CreateRegion(RT_DECKSTOCK, CRD_VISIBLE | CRD_3D, CRD_DONOTHING,
                 CRD_DONOTHING, CRD_OSYMBOL, 0, 0, 2, 2);
    // index 1-7
    for (int i = 1; i <= 7; i++) {
        CreateRegion(RegionType::RT_TABLEAU,
                     CRD_VISIBLE | CRD_DODRAG | CRD_DODROP,
                     CRD_DOOPCOLOR | CRD_DOLOWER | CRD_DOLOWERBY1 | CRD_DOKING,
                     CRD_DRAGFACEUP, CRD_HSYMBOL, 0, 0, 0, 0);
    }

    // index 8 (deck face up)
    CreateRegion(RegionType::RT_DECKSTOCK_FACEUP,
                 CRD_VISIBLE | CRD_FACEUP | CRD_DODRAG | CRD_3D, CRD_DOALL,
                 CRD_DRAGTOP, CRD_NSYMBOL, 0, 0, 0, 0);

    // index 9-12 (4 aces place on the top)
    for (int i = 4; i <= 7; i++) {
        CreateRegion(RegionType::RT_ACE_FOUNDATION,
                     CRD_VISIBLE | CRD_3D | CRD_DODRAG | CRD_DODROP,
                     CRD_DOSINGLE | CRD_DOHIGHER | CRD_DOHIGHERBY1 | CRD_DOACE |
                         CRD_DOSUIT,
                     CRD_DRAGTOP, CRD_HSYMBOL, 0, 0, 0, 0);
    }

    repositionRegions(true);

    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        vir->SetDeckSurface(_p_DeckLoader->GetDeckSurface());
    }

    LPErrInApp err = newGame();
    if (err != NULL)
        return err;
    _state = SolitarioGfx::READY_TO_START;

    return NULL;
}

void SolitarioGfx::showYesNoMsgBox(LPCSTR strText) {
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    if (_p_MsgBox != NULL) {
        delete _p_MsgBox;
    }
    _p_MsgBox = new MesgBoxGfx();
    int offsetW = 100;
    int offsetH = 130;
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w - offsetW;
    rctBox.h = offsetH;
    rctBox.y = (_p_Screen->h - rctBox.h) / 2;
    rctBox.x = (_p_Screen->w - rctBox.w) / 2;

    _p_MsgBox->ChangeAlpha(150);
    _p_MsgBox->Initialize(&rctBox, _p_Screen, pGameSettings->GetFontDjvMedium(),
                          MesgBoxGfx::TY_MB_YES_NO, _fnUpdateScreen);
    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_AlphaDisplay, &clipRect);
    SDL_FillSurfaceRect(
        _p_AlphaDisplay, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_AlphaDisplay->format), NULL, 0,
                   0, 0));

    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    STRING strTextYes = pLanguages->GetStringId(Languages::ID_YES);
    STRING strTextNo = pLanguages->GetStringId(Languages::ID_NO);
    _p_MsgBox->Show(_p_AlphaDisplay, strTextYes.c_str(), strTextNo.c_str(),
                    strText);
}

void SolitarioGfx::showOkMsgBox(LPCSTR strText) {
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    if (_p_MsgBox != NULL) {
        delete _p_MsgBox;
    }
    _p_MsgBox = new MesgBoxGfx();

    int offsetW = 100;
    int offsetH = 130;
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w - offsetW;
    rctBox.h = offsetH;
    rctBox.y = (_p_Screen->h - rctBox.h) / 2;
    rctBox.x = (_p_Screen->w - rctBox.w) / 2;

    _p_MsgBox->ChangeAlpha(150);
    _p_MsgBox->Initialize(&rctBox, _p_Screen, pGameSettings->GetFontDjvMedium(),
                          MesgBoxGfx::TY_MBOK, _fnUpdateScreen);

    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_AlphaDisplay, &clipRect);
    SDL_FillSurfaceRect(
        _p_AlphaDisplay, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_AlphaDisplay->format), NULL, 0,
                   0, 0));

    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    STRING strTextOk = pLanguages->GetStringId(Languages::ID_OK);
    _p_MsgBox->Show(_p_AlphaDisplay, strTextOk.c_str(), "", strText);
}

void SolitarioGfx::BtQuitClick() {
    TRACE("Quit with user button\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    _stateAfterYes = SolitarioGfx::FADING_OUT;
    _stateAfterNo = _state;
    _state = SolitarioGfx::IN_MSGBOX;

    showYesNoMsgBox(pLanguages->GetCStringId(Languages::ASK_QUIT));
}

void SolitarioGfx::BtNewGameClick() {
    TRACE("New Game click on button\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    _stateAfterYes = SolitarioGfx::DO_NEWGAME;
    _stateAfterNo = _state;
    _state = SolitarioGfx::IN_MSGBOX;
    showYesNoMsgBox(pLanguages->GetCStringId(Languages::ASK_NEWGAME));
}

void SolitarioGfx::BtToggleSoundClick() {
    TRACE("Toggle Sound with user button\n");
    STRING strTextBt;
    if (_p_MusicManager->IsPlayingMusic()) {
        // playing music or paused
        if (_p_MusicManager->IsMusicPaused()) {
            _p_MusicManager->ResumeMusic();
            strTextBt = "🔊";
        } else {
            _p_MusicManager->PauseMusic();
            strTextBt = "🔇";
        }
    } else {
        // no music
        strTextBt = "🔇";
    }
    _p_BtToggleSound->SetButtonText(strTextBt.c_str());
    DrawStaticScene();
}

void SolitarioGfx::BtToggleFullscreenClick() {
    TRACE("Toggle Fullscreen with user button\n");
    Uint32 flags = SDL_GetWindowFlags(_p_Window);
    STRING strTextBt;
    if (flags & SDL_WINDOW_FULLSCREEN) {
        SDL_SetWindowFullscreen(_p_Window, false);
        strTextBt = "🗖";
    } else {
        SDL_SetWindowFullscreen(_p_Window, true);
        strTextBt = "✕";
    }
    _p_BtToggleFullscreen->SetButtonText(strTextBt.c_str());
    DrawStaticScene();
}

LPErrInApp SolitarioGfx::drawScore() {
    if (!_scoreChanged) {
        return NULL;
    }
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    int tx = 10;
    int offsetY = 30;
    if (_scoreGame < -2000) {
        _scoreGame = -2000;
    }
    int ty = _p_Screen->h - offsetY;
    char buff[256];
    snprintf(buff, sizeof(buff), "%s : %d",
             pLanguages->GetCStringId(Languages::ID_SCORE), (int)_scoreGame);

    SDL_Color colorText = GFX_UTIL_COLOR::White;
    if (_scoreGame < 0) {
        colorText = GFX_UTIL_COLOR::Red;
    }

    SDL_Rect rcs;
    rcs.x = tx - 2;
    rcs.w = tx + 190;
    rcs.y = ty - 2;
    rcs.h = ty + 46;
    SDL_FillSurfaceRect(_p_Screen, &rcs,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    LPErrInApp err = GFX_UTIL::DrawString(_p_Screen, buff, tx, ty, colorText,
                                          _p_FontBigText);
    if (err != NULL) {
        return err;
    }

    _scoreChanged = false;
    updateTextureAsFlipScreen();

    return err;
}

void SolitarioGfx::updateBadScoreScoreOnTime() {
    if (_p_currentTime->IsMoreThenOneSecElapsed()) {
        uint64_t deltaSec = _p_currentTime->GetDeltaFromLastUpdate();
        _scoreGame = _scoreGame - deltaSec;
        _scoreChanged = true;
    }
}

void SolitarioGfx::updateScoreOnAce(int sizeAce, int oldSizeAce) {
    if (sizeAce > oldSizeAce) {
        _scoreGame += 60 * (sizeAce - oldSizeAce);
        _scoreChanged = true;
    }
}

void SolitarioGfx::updateScoreOnTurnOverFaceDown() {
    _scoreGame += 25;
    _scoreChanged = true;
}

void SolitarioGfx::updateScoreMoveDeckToTableau() {
    _scoreGame += 45;
    _scoreChanged = true;
}

void SolitarioGfx::updateBadScoreRedial() {
    _scoreGame -= 175;
    _scoreChanged = true;
}

void SolitarioGfx::updateBadScoreAceToTableu() {
    _scoreChanged = true;
    _scoreGame -= 75;
}

void SolitarioGfx::updateTextureAsFlipScreen() {
    (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_Screen);
}

void SolitarioGfx::clearScore() {
    uint64_t oldScore = _scoreGame;
    _scoreGame = 0;
    _scoreChanged = (oldScore != _scoreGame);
}

void SolitarioGfx::bonusScore() {
    uint64_t bonus =
        (2 * _scoreGame) - (_p_currentTime->GetNumOfSeconds() * 10);
    if (bonus > 0) {
        _scoreGame += bonus;
        _scoreChanged = true;
    }
}
void SolitarioGfx::repositionRegions(bool initFaces) {
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    int xLine0 = 35;
    int yLine0 = 10;
    int yoffsetLine0 = 40;
    int yOverlapCard = 37;
    _scaleFactor = 0.0f;
    int xOffsetIntraStack = 17;
    int xOffsetFaceUp = 25;

    if (pGameSettings->IsNarrowPortrait()) {
        yLine0 = 30;
        yOverlapCard = 42;
        xOffsetIntraStack = 14;
        if (_deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
            _scaleFactor = 0.75f;
        }
    } else {
        if (_deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
            yOverlapCard = 32;
        }
    }

    int cardWidth = _p_DeckLoader->GetCardWidth();
    int cardHeight = _p_DeckLoader->GetCardHeight();
    if (_scaleFactor > 0.0f) {
        cardWidth = (int)(_p_DeckLoader->GetCardWidth() * _scaleFactor);
        cardHeight = (int)(_p_DeckLoader->GetCardHeight() * _scaleFactor);
        xOffsetIntraStack = 6;
        xOffsetFaceUp = 10;
    }

    if (_cardRegionList.size() < 13)
        return;

    // index 0 (deck with face down)
    _cardRegionList[DeckPile_Ix].SetX(xLine0);
    _cardRegionList[DeckPile_Ix].SetY(yLine0);
    _cardRegionList[DeckPile_Ix].SetScaleFactor(_scaleFactor);

    // index 1-7 (Tableaus)
    for (int i = 1; i <= 7; i++) {
        _cardRegionList[i].SetX((cardWidth * (i - 1)) +
                                (i * xOffsetIntraStack));
        _cardRegionList[i].SetY(cardHeight + yoffsetLine0 + yLine0);
        _cardRegionList[i].SetScaleFactor(_scaleFactor);
        _cardRegionList[i].SetyOffset(yOverlapCard);
    }

    // index 8 (deck face up)
    _cardRegionList[DeckFaceUp].SetX(xLine0 + cardWidth + xOffsetFaceUp);
    _cardRegionList[DeckFaceUp].SetY(yLine0);
    _cardRegionList[DeckFaceUp].SetScaleFactor(_scaleFactor);

    // index 9-12 (4 aces)
    for (int i = 4; i <= 7; i++) {
        int idx = i + 5;
        _cardRegionList[idx].SetX((cardWidth * (i - 1)) +
                                  (i * xOffsetIntraStack));
        _cardRegionList[idx].SetY(yLine0);
        _cardRegionList[idx].SetScaleFactor(_scaleFactor);
    }

    InitAllCoords(initFaces);
}
