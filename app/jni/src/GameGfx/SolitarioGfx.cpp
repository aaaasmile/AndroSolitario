#include "SolitarioGfx.h"

#include <SDL3/SDL_endian.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

#include "CompGfx/ButtonGfx.h"
#include "CompGfx/MesgBoxGfx.h"
#include "CurrentTime.h"
#include "Fading.h"
#include "GfxUtil.h"
#include "HighScore.h"
#include "MusicManager.h"
#include "WinTypeGlobal.h"

// card graphics
int g_CardWidth = 0;
int g_CardHeight = 0;
int g_SymbolWidth = 0;
int g_SymbolHeight = 0;

static const char* g_lpszSymbDir = DATA_PREFIX "images/";
extern const char* g_lpszDeckDir;

const int MYIDQUIT = 0;
const int MYIDNEWGAME = 1;
const int MYIDTOGGLESOUND = 2;

SolitarioGfx::SolitarioGfx() {
    _p_ScreenBackbufferDrag = NULL;
    _startdrag = false;
    _p_Dragface = NULL;
    _p_SceneBackground = NULL;
    _p_ScreenTexture = NULL;
    _p_BtQuit = NULL;
    _p_BtNewGame = NULL;
    _p_BtToggleSound = NULL;
    _p_AlphaDisplay = NULL;
    _newgamerequest = false;
    _terminated = true;
    _p_currentTime = new CurrentTime();
}

SolitarioGfx::~SolitarioGfx() {
    CleanUpRegion();
    clearSurface();
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
    delete _p_currentTime;
}

void SolitarioGfx::clearSurface() {
    TRACE_DEBUG("clearSurface\n");
    if (_p_ScreenBackbufferDrag != NULL) {
        SDL_DestroySurface(_p_ScreenBackbufferDrag);
        _p_ScreenBackbufferDrag = NULL;
    }
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
        _p_ScreenTexture = NULL;
    }
    if (_p_AlphaDisplay != NULL) {
        SDL_DestroySurface(_p_AlphaDisplay);
        _p_AlphaDisplay = NULL;
    }
}

// command buttons
// -- quit --
void fncBind_ButtonQuitClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtQuitClick();
}

ClickCb SolitarioGfx::prepClickQuitCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonQuitClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

// -- new game --
void fncBind_ButtonNewGameClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtNewGameClick();
}

ClickCb SolitarioGfx::prepClickNewGameCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonNewGameClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

// -- sound on/off --
void fncBind_ButtonToggleSoundClick(void* self, int val) {
    SolitarioGfx* pApp = (SolitarioGfx*)self;
    pApp->BtToggleSoundClick();
}

ClickCb SolitarioGfx::prepClickToggleSoundCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonToggleSoundClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

LPErrInApp SolitarioGfx::Initialize(SDL_Surface* s, SDL_Renderer* r,
                                    SDL_Window* w,
                                    SDL_Surface* pSceneBackground,
                                    HighScore* pHighScore) {
    TRACE("Initialize Solitario\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    setDeckType(pGameSettings->DeckTypeVal);
    _p_MusicManager = pGameSettings->GetMusicManager();
    _p_HighScore = pHighScore;
    _sceneBackgroundIsBlack =
        pGameSettings->BackgroundType == BackgroundTypeEnum::Black;
    _p_FontBigText = pGameSettings->GetFontAriblk();
    _p_FontSmallText = pGameSettings->GetFontVera();
    LPErrInApp err;
    _p_Screen = s;
    _p_sdlRenderer = r;
    _p_Window = w;
    _p_ScreenTexture = SDL_CreateTexture(
        r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, s->w, s->h);
    if (_p_ScreenTexture == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot create texture: %s\n",
                                     SDL_GetError());
    }

    // _p_AlphaDisplay = SDL_CreateRGBSurface(SDL_SWSURFACE, _p_Screen->w,
    //                                        _p_Screen->h, 32, 0, 0, 0, 0);
    //                                        //SDL2
    _p_AlphaDisplay = GFX_UTIL::SDL_CreateRGBSurface(_p_Screen->w, _p_Screen->h,
                                                     32, 0, 0, 0, 0);
    if (_p_AlphaDisplay == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot create alpha display: %s\n",
                                     SDL_GetError());
    }

    // _p_ScreenBackbufferDrag = SDL_CreateRGBSurface(
    //     SDL_SWSURFACE, _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);//SDL2
    _p_ScreenBackbufferDrag = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    _p_SceneBackground = pSceneBackground;
    if (_p_SceneBackground == 0) {
        return ERR_UTIL::ErrorCreate("Cannot create scene background: %s\n",
                                     SDL_GetError());
    }
    if (_deckType.IsPacType()) {
        TRACE("Deck Pac stuff\n");
        err = LoadCardPac();
        if (err != NULL) {
            return err;
        }
        err = LoadSymbolsForPac();
        if (err != NULL) {
            return err;
        }
    } else {
        return ERR_UTIL::ErrorCreate("Only pac file supported");
    }

    return DrawInitialScene();
}

void SolitarioGfx::InitAllCoords() {
    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        vir->InitCardCoords();
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
    // TRACE_DEBUG("Draw card stack %d, visible %d\n", pcardRegion->RegionTypeId(),
    //             pcardRegion->IsVisible());

    LPErrInApp err;
    if (!pcardRegion->IsVisible())
        return NULL;

    DrawSymbol(pcardRegion->X(), pcardRegion->Y(), pcardRegion->Symbol());
    for (int i = 0; i < pcardRegion->Size(); i++) {
        LPCardGfx pCard = pcardRegion->Item(i);
        if (pCard->IsFaceUp()) {
            err = DrawCard(pCard, s);
        } else {
            err = DrawCardBack(pCard->X(), pCard->Y(), s);
        }
        if (err != NULL) {
            return err;
        }
    }
    return NULL;
}

void SolitarioGfx::CreateRegion(RegionType rtid, unsigned int attribs,
                                unsigned int amode, int dmode, int symbol,
                                int x, int y, int xoffset, int yoffset) {
    CardRegionGfx* cr =
        new CardRegionGfx(rtid, attribs, amode, dmode, symbol, x, y, xoffset,
                          yoffset, _deckType, g_CardWidth, g_CardHeight);
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

LPErrInApp SolitarioGfx::InitDrag(LPCardStackGfx pCargoStack, int x, int y,
                                  bool& isInitDrag,
                                  LPCardRegionGfx pSrcRegion) {
    TRACE_DEBUG("  InitDrag - start \n");
    isInitDrag = false;
    LPErrInApp err;

    if (pCargoStack == NULL) {
        if (_p_selectedCardRegion->IsEmpty())
            return NULL;

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

    TRACE_DEBUG("  InitDrag - DrawStaticScene - start \n");
    DrawStaticScene();
    TRACE_DEBUG("  InitDrag - DrawStaticScene - end \n");
    CardRegionGfx dragRegion(
        RegionType::RT_DRAG_REGION,
        _p_selectedCardRegion->GetAttributes() | CRD_FACEUP, 0, 0, 0, 0, 0,
        _p_selectedCardRegion->GetxOffset(),
        _p_selectedCardRegion->GetyOffset(), _deckType, g_CardWidth,
        g_CardHeight);

    dragRegion.PushStack(&_dragStack);

    _dragPileInfo.x = _dragStack.First()->X();
    _dragPileInfo.y = _dragStack.First()->Y();
    _dragPileInfo.width = dragRegion.GetStackWidth();
    _dragPileInfo.height = dragRegion.GetStackHeight();
    _dragPileInfo.pSrcRegion = pSrcRegion;

    if (_p_Dragface != NULL) {
        SDL_DestroySurface(_p_Dragface);
    }

    // _p_Dragface = SDL_CreateRGBSurface(SDL_SWSURFACE, _dragPileInfo.width,
    //                                    _dragPileInfo.height, 32, 0, 0, 0, 0);
    //                                    // SDL2
    _p_Dragface = GFX_UTIL::SDL_CreateRGBSurface(
        _dragPileInfo.width, _dragPileInfo.height, 32, 0, 0, 0, 0);
    // SDL_FillRect(_p_Dragface, NULL, SDL_MapRGB(_p_Dragface->format, 0, 255,
    // 0)); //SDL2
    SDL_FillSurfaceRect(
        _p_Dragface, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Dragface->format), NULL, 0, 255,
                   0));

    // SDL_SetColorKey(_p_Dragface, true,
    //                 SDL_MapRGB(_p_Dragface->format, 0, 255, 0)); // SDL2
    SDL_SetSurfaceColorKey(
        _p_Dragface, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Dragface->format), NULL, 0, 255,
                   0));

    err = DrawCardStack(_p_Screen, &dragRegion);
    if (err != NULL) {
        return err;
    }
    dragRegion.InitCardCoords();
    err = DrawCardStack(_p_Dragface, &dragRegion);
    if (err != NULL) {
        return err;
    }

    _oldx = x;
    _oldy = y;

    updateTextureAsFlipScreen();
    isInitDrag = true;
    TRACE_DEBUG("  InitDrag - end \n");
    return NULL;
}

void SolitarioGfx::updateTextureAsFlipScreen() {
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    // SDL_RenderCopy(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL); SDL 2
    SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);
}

void SolitarioGfx::DoDrag(int x, int y) {
    TRACE_DEBUG(
        "DoDrag (x=%d, y=%d) => drag_x=%d, drag_y=%d. old_x=%d, old_y=%d\n", x,
        y, _dragPileInfo.x, _dragPileInfo.y, _oldx, _oldy);
    SDL_Rect rcs;
    SDL_Rect rcd;

    rcs.x = _dragPileInfo.x - 2;
    rcs.y = _dragPileInfo.y - 2;
    rcs.w = _dragPileInfo.width + 4;
    rcs.h = _dragPileInfo.height + 4;

    rcd.x = _dragPileInfo.x - 2;
    rcd.y = _dragPileInfo.y - 2;

    if (_dragPileInfo.x < 0)
        rcs.x = rcd.x = 0;
    if (_dragPileInfo.y < 0)
        rcs.y = rcd.y = 0;

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

    SDL_Rect dest;
    dest.x = _dragPileInfo.x;
    dest.y = _dragPileInfo.y;

    SDL_BlitSurface(_p_ScreenBackbufferDrag, &rcs, _p_Screen, &rcd);
    SDL_BlitSurface(_p_Dragface, NULL, _p_Screen, &dest);

    updateTextureAsFlipScreen();
}

LPCardRegionGfx SolitarioGfx::DoDrop() { return DoDrop(NULL); }

LPCardRegionGfx SolitarioGfx::DoDrop(LPCardRegionGfx pDestRegion) {
    TRACE_DEBUG("DoDrop -  start \n");
    LPCardStackGfx pDestStack;
    LPCardRegionGfx pBestRegion;

    if (pDestRegion != NULL)
        pBestRegion = pDestRegion;
    else
        pBestRegion = GetBestStack(_dragPileInfo.x, _dragPileInfo.y,
                                   g_CardWidth, g_CardHeight, &_dragStack);
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

    if (_dragPileInfo.x == pCard->X() && _dragPileInfo.y == pCard->Y()) {
        TRACE_DEBUG("DoDrop -  return on no movement \n");
        DrawStaticScene();
        return pBestRegion;  // when no movement
    }

    zoomDropCard(_dragPileInfo.x, _dragPileInfo.y, pCard, _dragPileInfo.width,
                 _dragPileInfo.height);

    SDL_DestroySurface(_p_Dragface);
    _p_Dragface = NULL;

    TRACE_DEBUG("DoDrop -  end \n");
    return pBestRegion;
}

void calcPt(int x0, int y0, int x1, int y1, float t, int& xf, int& yf) {
    xf = int(x0 + t * (x1 - x0) + .5);
    yf = int(y0 + t * (y1 - y0) + .5);
}

void SolitarioGfx::zoomDropCard(int& sx, int& sy, LPCardGfx pCard, int w,
                                int h) {
    // TRACE("Zoom card x=%d, y=%d", sx, sy);
    SDL_Rect rcs;
    SDL_Rect rcd;
    SDL_Rect dest;

    int px, py;
    int dx = pCard->X();
    int dy = pCard->Y();
    float precision = 0.1f;
    Uint64 FPS = 20;                                
    for (float i = 0.0; i <= 1.0; i += precision) {
        Uint64 last_time = SDL_GetTicks();
        SDL_PumpEvents();
        calcPt(sx, sy, dx, dy, i, px, py);

        rcs.x = sx - 2;
        rcs.y = sy - 2;
        rcs.w = w + 4;
        rcs.h = h + 4;

        rcd.x = sx - 2;
        rcd.y = sy - 2;

        if (sx < 0)
            rcs.x = rcd.x = 0;
        if (sy < 0)
            rcs.y = rcd.y = 0;

        sx = dest.x = px;
        sy = dest.y = py;

        SDL_BlitSurface(_p_ScreenBackbufferDrag, &rcs, _p_Screen, &rcd);
        SDL_BlitSurface(_p_Dragface, NULL, _p_Screen, &dest);
        Uint64 now_time = SDL_GetTicks();
        if (now_time < last_time + FPS) {
            Uint32 delay = last_time + FPS - now_time;
            SDL_Delay(delay);
        }
        updateTextureAsFlipScreen();
    }
    _scoreChanged = true;
    DrawStaticScene();
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
    // Then the screen is copied into the back buffer for animations
    // SDL_FillRect(_p_Screen, &_p_Screen->clip_rect,
    // SDL_MapRGBA(_p_Screen->format, 0, 0, 0, 0));// SDL 2
    SDL_Rect clipRect;  // SDL 3
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    SDL_FillSurfaceRect(_p_Screen, &clipRect,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    SDL_Rect rctTarget;
    rctTarget.x = (_p_Screen->w - _p_SceneBackground->w) / 2;
    rctTarget.y = (_p_Screen->h - _p_SceneBackground->h) / 2;
    rctTarget.w = _p_SceneBackground->w;
    rctTarget.h = _p_SceneBackground->h;
    SDL_BlitSurface(_p_SceneBackground, NULL, _p_Screen, &rctTarget);

    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        SDL_PumpEvents();
        CardRegionGfx cardRegion = *vir;
        DrawCardStack(&cardRegion);
    }
    _p_BtNewGame->DrawButton(_p_Screen);
    _p_BtQuit->DrawButton(_p_Screen);
    _p_BtToggleSound->DrawButton(_p_Screen);
    _scoreChanged = true;
    drawScore(_p_Screen);
    // it seems here that SDL_BlitSurface copy only the bitmap and not the fill
    // rect, do it also into the backbuffer
    // SDL_FillRect(_p_ScreenBackbufferDrag,
    // &_p_ScreenBackbufferDrag->clip_rect,
    // SDL_MapRGBA(_p_ScreenBackbufferDrag->format, 0, 0, 0, 0)); //SDL 2
    // SDL 3
    SDL_GetSurfaceClipRect(_p_ScreenBackbufferDrag, &clipRect);
    SDL_FillSurfaceRect(
        _p_ScreenBackbufferDrag, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_ScreenBackbufferDrag->format),
                   NULL, 0, 0, 0));

    SDL_BlitSurface(_p_Screen, NULL, _p_ScreenBackbufferDrag, NULL);

    updateTextureAsFlipScreen();
}

LPErrInApp SolitarioGfx::DrawInitialScene() {
    TRACE("DrawInitialScene\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    // SDL_FillRect(_p_Screen, &_p_Screen->clip_rect,
    // SDL_MapRGBA(_p_Screen->format, 0, 0, 0, 0)); //SDL 2
    SDL_Rect clipRect;  // SDL 3
    SDL_GetSurfaceClipRect(_p_Screen, &clipRect);
    SDL_FillSurfaceRect(_p_Screen, &clipRect,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    SDL_Rect rctTarget;
    rctTarget.x = (_p_Screen->w - _p_SceneBackground->w) / 2;
    rctTarget.y = (_p_Screen->h - _p_SceneBackground->h) / 2;
    rctTarget.w = _p_SceneBackground->w;
    rctTarget.h = _p_SceneBackground->h;
    if (!_sceneBackgroundIsBlack) {
        if (pGameSettings->InputType != InputTypeEnum::TouchWithoutMouse) {
            Fade(_p_Screen, _p_SceneBackground, 2, 0, _p_sdlRenderer,
                 &rctTarget);
        } else {
            InstantFade(_p_Screen);
        }
    }

    SDL_Rect rctBt1;
    int btw = 120;
    int btwSymb = 60;
    int bth = 28;
    int btoffsetY = 70;
    int btposx = 150;
    int btintraX = 30;
    if (pGameSettings->NeedScreenMagnify()) {
        btw = 200;
        bth = 70;
        btoffsetY = 400;
        btposx = 500;
        btintraX = 50;
        btwSymb = 90;
    }
    if (_p_Screen->w == 1024) {
        btposx = 500;
    }
    // button Quit
    ClickCb cbBtQuit = prepClickQuitCb();
    _p_BtQuit = new ButtonGfx();
    rctBt1.w = btw;
    rctBt1.h = bth;
    rctBt1.y = _p_Screen->h - btoffsetY;
    rctBt1.x = btposx;
    _p_BtQuit->Initialize(&rctBt1, _p_Screen, _p_FontBigText, MYIDQUIT,
                          cbBtQuit);
    _p_BtQuit->SetVisibleState(ButtonGfx::INVISIBLE);

    // button new game
    ClickCb cbBtNewGame = prepClickNewGameCb();
    _p_BtNewGame = new ButtonGfx();
    rctBt1.x = rctBt1.x + rctBt1.w + btintraX;
    _p_BtNewGame->Initialize(&rctBt1, _p_Screen, _p_FontBigText, MYIDNEWGAME,
                             cbBtNewGame);
    _p_BtNewGame->SetVisibleState(ButtonGfx::INVISIBLE);
    // button toggle sound
    int tx = btposx;
    int offsetY = 30;
    if (pGameSettings->NeedScreenMagnify()) {
        tx = btposx;
        offsetY = 250;
    }
    rctBt1.y = _p_Screen->h - offsetY;
    rctBt1.x = tx;
    rctBt1.w = btwSymb;
    ClickCb cbBtToggleSound = prepClickToggleSoundCb();
    _p_BtToggleSound = new ButtonGfx();
    _p_BtToggleSound->InitializeAsSymbol(&rctBt1, _p_Screen,
                                         pGameSettings->GetFontSymb(),
                                         MYIDTOGGLESOUND, cbBtToggleSound);
    _p_BtToggleSound->SetVisibleState(ButtonGfx::INVISIBLE);
    return NULL;
}

LPCardRegionGfx SolitarioGfx::GetBestStack(int x, int y, int w, int h,
                                           LPCardStackGfx pStack) {
    int maxoverlap = 0;
    int percent = 0;
    LPCardRegionGfx pBest = NULL;

    for (regionVI vir = _cardRegionList.begin(); vir != _cardRegionList.end();
         ++vir) {
        SDL_PumpEvents();
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
    return DrawCard(x, y, nCdIndex, _p_Screen);
}

LPErrInApp SolitarioGfx::DrawCard(int x, int y, int nCdIndex, SDL_Surface* s) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Draw a card on NULL surface. This is wrong");
    }
    return DrawCardPac(x, y, nCdIndex, s);
}

LPErrInApp SolitarioGfx::DrawCardPac(int x, int y, int nCdIndex,
                                     SDL_Surface* s) {
    if (nCdIndex < 0 || nCdIndex >= _deckType.GetNumCards())
        return ERR_UTIL::ErrorCreate("DrawCardPac %d index out of range",
                                     nCdIndex);

    int suitIx = nCdIndex / _deckType.GetNumCardInSuit();
    int cardIx = nCdIndex % _deckType.GetNumCardInSuit();
    // TRACE_DEBUG("Suit %d, card: %d\n", suitIx, cardIx);

    SDL_Rect srcCard;
    srcCard.x = suitIx * g_CardWidth;
    srcCard.y = cardIx * g_CardHeight;
    srcCard.w = g_CardWidth;
    srcCard.h = g_CardHeight;

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    if (!SDL_BlitSurface(_p_Deck, &srcCard, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardPac with Iterator error: %s\n",
            SDL_GetError());
    }
    return NULL;
}

LPErrInApp SolitarioGfx::DrawCard(LPCardGfx pCard, SDL_Surface* s) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate("Error in draw card, surface is NULL\n");
    }
    // TRACE("Draw card ix = %d, suit = %s, rank %d, x,y %d,%d", pCard->Index(),
    //       pCard->SuitStr(), pCard->Rank(), pCard->X(), pCard->Y());
    return DrawCardPac(pCard, s);
}

LPErrInApp SolitarioGfx::DrawCardPac(LPCardGfx pCard, SDL_Surface* s) {
    pCard->SetDeckSurface(_p_Deck);

    return pCard->DrawCardPac(s);
}

LPErrInApp SolitarioGfx::DrawCardBack(int x, int y) {
    return DrawCardBack(x, y, _p_Screen);
}

LPErrInApp SolitarioGfx::DrawCardBack(int x, int y, SDL_Surface* s) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Error in DrawCardBack, surface is NULL\n");
    }

    return DrawCardBackPac(x, y, s);
}

LPErrInApp SolitarioGfx::DrawCardBackPac(int x, int y, SDL_Surface* s) {
    SDL_Rect dest, srcBack;
    dest.x = x;
    dest.y = y;

    srcBack.x = 0;
    srcBack.y = 0;
    srcBack.w = g_SymbolWidth;
    srcBack.h = g_SymbolHeight;

    if (!SDL_BlitSurface(_p_Symbols, &srcBack, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardBackPac error: %s\n", SDL_GetError());
    }

    return NULL;
}

LPErrInApp SolitarioGfx::DrawSymbol(int x, int y, int nSymbol) {
    return DrawSymbol(x, y, nSymbol, _p_Screen);
}

LPErrInApp SolitarioGfx::DrawSymbol(int x, int y, int nSymbol, SDL_Surface* s) {
    if (nSymbol < 1) {
        return ERR_UTIL::ErrorCreate("Symbol index %d out of range", nSymbol);
    }
    if (nSymbol > 3)
        nSymbol = 3;

    return DrawSymbolPac(x, y, nSymbol, s);
}

LPErrInApp SolitarioGfx::DrawSymbolPac(int x, int y, int nSymbol,
                                       SDL_Surface* s) {
    SDL_Rect srcCard;
    srcCard.x = nSymbol * g_SymbolWidth;
    srcCard.y = 0;
    srcCard.w = g_SymbolWidth;
    srcCard.h = g_SymbolHeight;

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    if (!SDL_BlitSurface(_p_Symbols, &srcCard, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawSymbolPac error: %s\n", SDL_GetError());
    }

    return NULL;
}

LPErrInApp SolitarioGfx::VictoryAnimation() {
    TRACE("Victory animation \n");
    LPErrInApp err;
    int rotation;
    int id;
    int x;
    unsigned int y;
    int xspeed;
    int yspeed;

    int gravity = 1;
    unsigned int max_y = _p_Screen->h;
    float bounce = 0.8f;
    SDL_Event event;
    Uint64 uiInitialTick = SDL_GetTicks();
    Uint64 uiLast_time = uiInitialTick;
    int FPS = 5;

    while (1) {
        rotation = rand() % 2;
        id = rand() % _deckType.GetNumCards();
        x = rand() % _p_Screen->w;
        y = rand() % _p_Screen->h / 2;

        if (rotation)
            xspeed = -4;
        else
            xspeed = 4;

        yspeed = 0;

        do {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_QUIT:
                        return NULL;
                    case SDL_EVENT_KEY_DOWN:
                        if (event.key.key == SDLK_ESCAPE) {
                            return NULL;
                        }
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        return NULL;
                    case SDL_EVENT_FINGER_DOWN:
                        return NULL;
                }
            }
            yspeed = yspeed + gravity;
            x += xspeed;
            y += yspeed;

            if (y + g_CardHeight > max_y) {
                y = max_y - g_CardHeight;
                yspeed = int(-yspeed * bounce);
            }
            err = DrawCard(x, y, id, _p_Screen);
            if (err != NULL) {
                return err;
            }
            updateTextureAsFlipScreen();
            // synch to frame rate
            Uint64 uiNowTime = SDL_GetTicks();
            if (uiNowTime < uiLast_time + FPS) {
                Uint32 delay = uiLast_time + FPS - uiNowTime;
                // TRACE_DEBUG("[Animaytion] Delay %d\n", delay);
                SDL_Delay(delay);
                uiLast_time = SDL_GetTicks();
            }
        } while ((x + g_CardWidth > 0) && (x < _p_Screen->w));
    }
    return NULL;
}

LPErrInApp SolitarioGfx::LoadCardPac() {
    Uint16 w, h;
    LPErrInApp err = GFX_UTIL::LoadCardPac(&_p_Deck, _deckType, &w, &h);
    if (err != NULL) {
        return err;
    }
    TRACE("Pac size  w = %d, h = %d\n", w, h);
    g_CardWidth = w / 4;
    g_CardHeight = h / _deckType.GetNumCardInSuit();
    return NULL;
}

LPErrInApp SolitarioGfx::LoadSymbolsForPac() {
    std::string strFileSymbName = g_lpszSymbDir;
    Uint8 r, g, b;
    strFileSymbName += _deckType.GetSymbolFileName();
    if (_deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
        // SDL_RWops *srcSymb = SDL_RWFromFile(strFileSymbName.c_str(), "rb");
        // // SDL 2
        SDL_IOStream* srcSymb =
            SDL_IOFromFile(strFileSymbName.c_str(), "rb");  // SDL 3
        if (srcSymb == NULL) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWFromFile on symbols failed: %s\n", SDL_GetError());
        }
        //_p_Symbols = IMG_LoadPNG_RW(srcSymb); SDL 2
        _p_Symbols = IMG_LoadTyped_IO(srcSymb, false, "PNG");
        if (_p_Symbols == NULL) {
            SDL_CloseIO(srcSymb);
            return ERR_UTIL::ErrorCreate(
                "IMG_LoadPNG_RW on symbols file error (file %s): %s\n",
                strFileSymbName.c_str(), SDL_GetError());
        }
        // SDL_SetColorKey(_p_Symbols, true,
        //                 SDL_MapRGB(_p_Symbols->format, 248, 0, 241)); // SDL
        //                 2
        r = 248;
        g = 0;
        b = 241;
        SDL_CloseIO(srcSymb);
    } else {
        _p_Symbols = SDL_LoadBMP(strFileSymbName.c_str());
        if (_p_Symbols == 0) {
            return ERR_UTIL::ErrorCreate("Load bitmap failed: %s\n",
                                         SDL_GetError());
        }
        if (_deckType.GetSymbolFileName() == "symb_336.bmp") {
            // SDL_SetColorKey(_p_Symbols, true,
            //                 SDL_MapRGB(_p_Symbols->format, 242, 30, 206));
            //                 SDL 2
            r = 242;
            g = 30;
            b = 206;
        } else {
            // SDL_SetColorKey(_p_Symbols, true,
            //                 SDL_MapRGB(_p_Symbols->format, 0, 128, 0)); SDL 2
            r = 0;
            g = 128;
            b = 0;
        }
    }
    SDL_SetSurfaceColorKey(
        _p_Symbols, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Symbols->format), NULL, r, g,
                   b));

    g_SymbolWidth = _p_Symbols->w / 4;
    g_SymbolHeight = _p_Symbols->h;

    return NULL;
}

LPErrInApp SolitarioGfx::newGame() {
    TRACE("New Game\n");
    LPErrInApp err;
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
    for (i = Found_Ix1; i <= Found_Ix7; i++) {
        LPCardStackGfx pStack = PopStackFromRegion(DeckPile_Ix, i);
        PushStackInRegion(i, pStack);
        delete pStack;
    }

    InitAllCoords();

    for (i = Found_Ix1; i <= Found_Ix7; i++) {
        SetCardFaceUp(i, true, RegionSize(i) - 1);
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopKeyDownEvent(SDL_Event& event) {
    LPErrInApp err;
    if (event.key.key == SDLK_N) {
        _newgamerequest = true;
    }
    if (event.key.key == SDLK_A) {
        err = VictoryAnimation();
        if (err != NULL) {
            return err;
        }
        TRACE("Exit from victory animation \n");
        DrawStaticScene();
    }
    return NULL;
}

// Remember Finger events are parallel with mouse events
// here use Finger only for button and mouse right click simulation
LPErrInApp SolitarioGfx::handleGameLoopFingerDownEvent(SDL_Event& event) {
    TRACE_DEBUG("handleGameLoopFingerDownEvent \n");
    _p_BtQuit->FingerDown(event);
    _p_BtNewGame->FingerDown(event);
    _p_BtToggleSound->FingerDown(event);
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopFingerUpEvent(SDL_Event& event) {
    TRACE_DEBUG("handleGameLoopFingerUpEvent (tms %d) \n",
                event.tfinger.timestamp);
    if (_lastUpTimestamp + 400 > event.tfinger.timestamp) {
        TRACE_DEBUG("Double Tap recognized\n");
        LPErrInApp err;
        CardRegionGfx* pRegion;
        bool isInitDrag = false;
        SDL_Point pt;
        LPGameSettings pGameSettings = GameSettings::GetSettings();
        pGameSettings->GetTouchPoint(event.tfinger,
                                     &pt);  // rememeber here event.button.x and
                                            // event.button.y will not works

        pRegion = SelectRegionOnPoint(pt.x, pt.y);

        if (pRegion == NULL)
            return NULL;
        TRACE_DEBUG("[DT] Region found %d\n", pRegion->Size());

        LPCardGfx pCard = pRegion->GetCard(pRegion->Size() - 1);
        if (pCard == NULL) {
            return NULL;
        }
        TRACE_DEBUG("[DT] Card found %s\n", pCard->Name());

        if (((pRegion->RegionTypeId() == RegionType::RT_TABLEAU) ||
             (pRegion->RegionTypeId() == RegionType::RT_DECKSTOCK_FACEUP)) &&
            pCard->IsFaceUp() && pRegion->PtOnTop(pt.x, pt.y)) {
            TRACE_DEBUG("[DT] Select card on region %s \n", pCard->Name());
            LPCardRegionGfx pDropRegion =
                FindDropRegion(RegionType::RT_ACE_FOUNDATION, pCard);
            if (pDropRegion == NULL) {
                return NULL;
            }
            TRACE_DEBUG("[DT] found drop region x=%d,y=%d \n", pDropRegion->X(),
                        pDropRegion->Y());
            LPCardStackGfx pCardStack = pRegion->PopStack(1);
            if (pCardStack == NULL) {
                return NULL;
            }
            TRACE_DEBUG("[DT] found cardStack size=%d \n", pCardStack->Size());
            err = InitDrag(pCardStack, -1, -1, isInitDrag, pRegion);
            if (err != NULL) {
                return err;
            }

            DoDrop(pDropRegion);
            updateScoreOnAce(pDropRegion->Size(), pDropRegion->GetSavedSize());
            delete pCardStack;
        }
    }
    _lastUpTimestamp = event.tfinger.timestamp;
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopMouseDownEvent(SDL_Event& event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        return handleLeftMouseDown(event);
    } else if (event.button.button == SDL_BUTTON_RIGHT) {
        return handleRightMouseDown(event);
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleLeftMouseDown(SDL_Event& event) {
    TRACE_DEBUG("handleLeftMouseDown \n");
    LPErrInApp err;
    CardRegionGfx* srcReg;
    bool isInitDrag = false;
    srcReg = SelectRegionOnPoint(event.button.x, event.button.y);
    if (srcReg == NULL)
        return NULL;
    if ((srcReg->RegionTypeId() == RegionType::RT_TABLEAU) &&
        srcReg->PtOnTop(event.button.x, event.button.y)) {
        int id = srcReg->Size() - 1;
        if (!srcReg->IsCardFaceUp(id)) {
            srcReg->SetCardFaceUp(true, id);
            updateScoreOnTurnOverFaceDown();
        }
    }

    if ((srcReg->RegionTypeId() == RegionType::RT_TABLEAU) ||
        (srcReg->RegionTypeId() == RegionType::RT_DECKSTOCK_FACEUP) ||
        (srcReg->RegionTypeId() == RegionType::RT_ACE_FOUNDATION)) {
        // clicked on region that can do dragging
        err = InitDrag(event.button.x, event.button.y, isInitDrag, srcReg);
        if (err != NULL) {
            return err;
        }
        if (isInitDrag) {
            _startdrag = true;
            // SDL_ShowCursor(SDL_DISABLE); SDL 2
            // SDL_SetWindowGrab(_p_Window, SDL_TRUE); SDL 2
            SDL_HideCursor();
            SDL_SetWindowMouseGrab(_p_Window, true);
        }
    } else if (srcReg->RegionTypeId() == RegionType::RT_DECKSTOCK) {
        if (srcReg->IsEmpty() && !IsRegionEmpty(DeckFaceUp)) {
            // from deckfaceup back to deckpile: drag and drop to deckpile
            // (redial)
            LPCardStackGfx pCardStack = PopStackFromRegion(
                eRegionIx::DeckFaceUp, RegionSize(eRegionIx::DeckFaceUp));
            pCardStack->SetCardsFaceUp(false);
            err = InitDrag(pCardStack, -1, -1, isInitDrag, srcReg);
            if (err != NULL) {
                return err;
            }
            DoDrop(GetRegion(DeckPile_Ix));
            Reverse(DeckPile_Ix);
            InitCardCoords(DeckPile_Ix);
            delete pCardStack;
            updateBadScoreRedial();
        } else if (!srcReg->IsEmpty()) {
            // the next card goes to the deck face up region: drag and drop to
            // deck face up
            LPCardStackGfx pCardStack = PopStackFromRegion(DeckPile_Ix, 1);
            pCardStack->SetCardsFaceUp(true);
            err = InitDrag(pCardStack, -1, -1, isInitDrag, srcReg);
            if (err != NULL) {
                return err;
            }
            DoDrop(GetRegion(DeckFaceUp));
            delete pCardStack;
        } else {
            TRACE("No more card on the pile deck\n");
        }
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleRightMouseDown(SDL_Event& event) {
    TRACE_DEBUG("handleRightMouseDown \n");
    LPErrInApp err;
    CardRegionGfx* srcReg;
    bool isInitDrag = false;
    srcReg = SelectRegionOnPoint(event.button.x, event.button.y);
    if (srcReg == NULL)
        return NULL;
    LPCardGfx pCard = srcReg->GetCard(srcReg->Size() - 1);
    if (pCard == NULL) {
        return NULL;
    }

    if (((srcReg->RegionTypeId() == RegionType::RT_TABLEAU) ||
         (srcReg->RegionTypeId() == RegionType::RT_DECKSTOCK_FACEUP)) &&
        pCard->IsFaceUp() && srcReg->PtOnTop(event.button.x, event.button.y)) {
        LPCardRegionGfx pDropRegion =
            FindDropRegion(RegionType::RT_ACE_FOUNDATION, pCard);
        if (pDropRegion == NULL) {
            return NULL;
        }
        LPCardStackGfx pCardStack = srcReg->PopStack(1);
        if (pCardStack == NULL) {
            return NULL;
        }
        err = InitDrag(pCardStack, -1, -1, isInitDrag, srcReg);
        if (err != NULL) {
            return err;
        }

        DoDrop(pDropRegion);
        updateScoreOnAce(pDropRegion->Size(), pDropRegion->GetSavedSize());
        delete pCardStack;
    }
    return NULL;
}

void SolitarioGfx::handleGameLoopMouseMoveEvent(SDL_Event& event) {
    // TRACE_DEBUG("handleGameLoopMouseMoveEvent \n");
    if (event.motion.state == SDL_BUTTON_MASK(1) && _startdrag) {
        DoDrag(event.motion.x, event.motion.y);
    }
    bool statusChanged = _p_BtNewGame->MouseMove(event);
    statusChanged = statusChanged || _p_BtQuit->MouseMove(event) ||
                    _p_BtToggleSound->MouseMove(event);
    if (statusChanged) {
        DrawStaticScene();
    }
}

LPErrInApp SolitarioGfx::handleGameLoopMouseUpEvent(SDL_Event& event) {
    TRACE_DEBUG("handleGameLoopMouseUpEvent \n");
    _p_BtQuit->MouseUp(event);
    _p_BtNewGame->MouseUp(event);
    _p_BtToggleSound->MouseUp(event);

    if (_startdrag) {
        // TRACE_DEBUG("MouseUp start drag - end \n");
        _startdrag = false;
        LPCardRegionGfx pDestReg = DoDrop();
        // SDL_ShowCursor(SDL_ENABLE);
        // SDL_SetWindowGrab(_p_Window, SDL_FALSE); SDL 2
        SDL_ShowCursor();
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
    }
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
        char buff[1024];
        LPGameSettings pGameSettings = GameSettings::GetSettings();
        LPLanguages pLanguages = pGameSettings->GetLanguageMan();
        snprintf(buff, 1024, pLanguages->GetCStringId(Languages::FINAL_SCORE),
                 _scoreGame);
        showOkMsgBox(buff);
        err = _p_HighScore->SaveScore(_scoreGame, _deckType.GetNumCards());
        if (err != NULL) {
            return err;
        }
        DrawStaticScene();
        VictoryAnimation();
        TRACE("Exit from victory animation \n");
        err = newGame();
        if (err != NULL) {
            return err;
        }
        DrawStaticScene();
    }

    return NULL;
}

LPErrInApp SolitarioGfx::StartGameLoop() {
    TRACE_DEBUG("StartGameLoop, card width %d, height %d\n", g_CardWidth,
                g_CardHeight);
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();

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
            strTextBt = "🔊";  //_p_Languages->GetStringId(Languages::ON);
        } else {
            strTextBt = "🔇";  //_p_Languages->GetStringId(Languages::OFF);
        }
        _p_BtToggleSound->SetButtonText(strTextBt.c_str());
        _p_BtToggleSound->SetVisibleState(ButtonGfx::VISIBLE);
    }

    int xLine0 = 35;
    int yLine0 = 10;
    int yoffsetLine0 = 40;
    int yOverlapCard = 32;
    int xOffsetIntraStack = 17;
    int xOffsetFaceUp = 25;
    if (pGameSettings->NeedScreenMagnify()) {
        if (g_CardWidth <= 127) {
            xLine0 = 50;
            yLine0 = 150;
            yoffsetLine0 = 150;
            yOverlapCard = 56;
            xOffsetIntraStack = 40;
            xOffsetFaceUp = 60;
        } else {
            xLine0 = 20;
            yLine0 = 120;
            yoffsetLine0 = 130;
            yOverlapCard = 40;
            xOffsetIntraStack = 24;
            xOffsetFaceUp = 48;
        }
    }

    // index 0 (deck with face down)
    CreateRegion(RT_DECKSTOCK,           // ID
                 CRD_VISIBLE | CRD_3D,   // attributes
                 CRD_DONOTHING,          // Accept mode
                 CRD_DONOTHING,          // drag mode
                 CRD_OSYMBOL,            // symbol
                 xLine0, yLine0, 2, 2);  // x, y, x offset, yoffset
    // index 1-7
    int i;
    for (i = 1; i <= 7; i++) {
        CreateRegion(RegionType::RT_TABLEAU,
                     CRD_VISIBLE | CRD_DODRAG | CRD_DODROP,  // attributes
                     CRD_DOOPCOLOR | CRD_DOLOWER | CRD_DOLOWERBY1 |
                         CRD_DOKING,  // accept mode
                     CRD_DRAGFACEUP,  // drag mode
                     CRD_HSYMBOL,     // symbol
                     (g_CardWidth * (i - 1)) + (i * xOffsetIntraStack),
                     g_CardHeight + yoffsetLine0 + yLine0, 0,
                     yOverlapCard);  // x, y, x offset, yoffset
    }

    // index 8 (deck face up)
    CreateRegion(RegionType::RT_DECKSTOCK_FACEUP,
                 CRD_VISIBLE | CRD_FACEUP | CRD_DODRAG | CRD_3D,  // Attributes
                 CRD_DOALL,                                       // accept mode
                 CRD_DRAGTOP,                                     // drag mode
                 CRD_NSYMBOL,                                     // symbol
                 xLine0 + g_CardWidth + xOffsetFaceUp, yLine0, 0,
                 0);  // x, y, x offset, yoffset

    // index 9-12 (4 aces place on the top)
    for (i = 4; i <= 7; i++) {
        CreateRegion(
            RegionType::RT_ACE_FOUNDATION,
            CRD_VISIBLE | CRD_3D | CRD_DODRAG | CRD_DODROP,  // Attributes
            CRD_DOSINGLE | CRD_DOHIGHER | CRD_DOHIGHERBY1 | CRD_DOACE |
                CRD_DOSUIT,  // Accept mode
            CRD_DRAGTOP,     // drop mode
            CRD_HSYMBOL,     // symbol
            (g_CardWidth * (i - 1)) + (i * xOffsetIntraStack), yLine0, 0,
            0);  // x, y, x offset, yoffset
    }

    LPErrInApp err = newGame();
    if (err != NULL)
        return err;
    DrawStaticScene();

    SDL_Event event;
    _terminated = false;
    while (!_terminated) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    Fade(_p_Screen, _p_Screen, 1, 1, _p_sdlRenderer, NULL);
                    return NULL;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        return NULL;
                    }
                    err = handleGameLoopKeyDownEvent(event);
                    if (err != NULL)
                        return err;
                    break;
                case SDL_EVENT_FINGER_DOWN:
                    // TRACE_DEBUG("Event SDL_EVENT_FINGER_DOWN \n");
                    err = handleGameLoopFingerDownEvent(event);
                    if (err != NULL)
                        return err;
                    break;
                case SDL_EVENT_FINGER_UP:
                    // TRACE_DEBUG("Event SDL_EVENT_FINGER_UP \n");
                    err = handleGameLoopFingerUpEvent(event);
                    if (err != NULL)
                        return err;
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    // TRACE_DEBUG("Event SDL_EVENT_MOUSE_BUTTON_DOWN - start
                    // \n");
                    err = handleGameLoopMouseDownEvent(event);
                    // TRACE_DEBUG("Event SDL_EVENT_MOUSE_BUTTON_DOWN - end
                    // \n");
                    if (err != NULL)
                        return err;
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    handleGameLoopMouseMoveEvent(event);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    // TRACE_DEBUG("Event SDL_EVENT_MOUSE_BUTTON_UP \n");
                    err = handleGameLoopMouseUpEvent(event);
                    if (err != NULL)
                        return err;
                    break;
            }
        }
        updateBadScoreScoreOnTime();
        // write direct into the screen because it could be that a dragging is
        // in action and the screen for a back buffer is dirty
        err = drawScore(_p_Screen);
        if (err != NULL)
            return err;
        if (_newgamerequest) {
            _newgamerequest = false;
            err = newGame();
            if (err != NULL) {
                return err;
            }
            DrawStaticScene();
        }
    }
    if (_p_MusicManager->IsPlayingMusic()) {
        _p_MusicManager->StopMusic(300);
    }
    return NULL;
}

int SolitarioGfx::showYesNoMsgBox(LPCSTR strText) {
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    MesgBoxGfx MsgBox;
    int offsetW = 100;
    int offsetH = 130;
    if (pGameSettings->NeedScreenMagnify()) {
        offsetW = 150;
        offsetH = 260;
    }
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w - offsetW;
    rctBox.h = offsetH;
    rctBox.y = (_p_Screen->h - rctBox.h) / 2;
    rctBox.x = (_p_Screen->w - rctBox.w) / 2;

    MsgBox.ChangeAlpha(150);
    MsgBox.Initialize(&rctBox, _p_Screen, _p_FontSmallText,
                      MesgBoxGfx::TY_MB_YES_NO, _p_sdlRenderer);
    DrawStaticScene();
    // SDL_FillRect(_p_AlphaDisplay, &_p_AlphaDisplay->clip_rect,
    //              SDL_MapRGBA(_p_AlphaDisplay->format, 0, 0, 0, 0)); SDL 2
    SDL_Rect clipRect;  // SDL 3
    SDL_GetSurfaceClipRect(_p_AlphaDisplay, &clipRect);
    SDL_FillSurfaceRect(
        _p_AlphaDisplay, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_AlphaDisplay->format), NULL, 0,
                   0, 0));

    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    STRING strTextYes = pLanguages->GetStringId(Languages::ID_YES);
    STRING strTextNo = pLanguages->GetStringId(Languages::ID_NO);

    return MsgBox.Show(_p_AlphaDisplay, strTextYes.c_str(), strTextNo.c_str(),
                       strText);
}

void SolitarioGfx::showOkMsgBox(LPCSTR strText) {
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    int offsetW = 100;
    int offsetH = 130;
    if (pGameSettings->NeedScreenMagnify()) {
        offsetW = 150;
        offsetH = 260;
    }
    MesgBoxGfx MsgBox;
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w - offsetW;
    rctBox.h = offsetH;
    rctBox.y = (_p_Screen->h - rctBox.h) / 2;
    rctBox.x = (_p_Screen->w - rctBox.w) / 2;

    MsgBox.ChangeAlpha(150);
    MsgBox.Initialize(&rctBox, _p_Screen, _p_FontSmallText, MesgBoxGfx::TY_MBOK,
                      _p_sdlRenderer);
    DrawStaticScene();
    // SDL_FillRect(_p_AlphaDisplay, &_p_AlphaDisplay->clip_rect,
    //              SDL_MapRGBA(_p_AlphaDisplay->format, 0, 0, 0, 0)); SDL 2
    SDL_Rect clipRect;  // SDL 3
    SDL_GetSurfaceClipRect(_p_AlphaDisplay, &clipRect);
    SDL_FillSurfaceRect(
        _p_AlphaDisplay, &clipRect,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_AlphaDisplay->format), NULL, 0,
                   0, 0));

    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    STRING strTextOk = pLanguages->GetStringId(Languages::ID_OK);
    MsgBox.Show(_p_AlphaDisplay, strTextOk.c_str(), "", strText);
}

void SolitarioGfx::BtQuitClick() {
    TRACE("Quit with user button\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    if (showYesNoMsgBox(pLanguages->GetCStringId(Languages::ASK_QUIT)) ==
        MesgBoxGfx::RES_YES) {
        _terminated = true;
    } else {
        DrawStaticScene();
    }
}

void SolitarioGfx::BtNewGameClick() {
    TRACE("New Game with user button\n");
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    if (showYesNoMsgBox(pLanguages->GetCStringId(Languages::ASK_NEWGAME)) ==
        MesgBoxGfx::RES_YES) {
        _newgamerequest = true;
    } else {
        DrawStaticScene();
    }
}

void SolitarioGfx::BtToggleSoundClick() {
    TRACE("Toggle Sound with user button\n");
    STRING strTextBt;
    if (_p_MusicManager->IsPlayingMusic()) {
        // playing music or paused
        if (_p_MusicManager->IsMusicPaused()) {
            _p_MusicManager->ResumeMusic();
            strTextBt = "🔊";  //_p_Languages->GetStringId(Languages::ON);
        } else {
            _p_MusicManager->PauseMusic();
            strTextBt = "🔇";  //_p_Languages->GetStringId(Languages::OFF);
        }
    } else {
        // no music
        strTextBt = "🔇";
    }
    _p_BtToggleSound->SetButtonText(strTextBt.c_str());
    DrawStaticScene();
}

LPErrInApp SolitarioGfx::drawScore(SDL_Surface* pScreen) {
    if (!_scoreChanged) {
        return NULL;
    }
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    LPLanguages pLanguages = pGameSettings->GetLanguageMan();
    int tx = 10;
    int offsetY = 30;
    if (pGameSettings->NeedScreenMagnify()) {
        tx = 100;
        offsetY = 400;
    }
    int ty = pScreen->h - offsetY;
    char buff[256];
    snprintf(buff, sizeof(buff), "%s : %d",
             pLanguages->GetCStringId(Languages::ID_SCORE), _scoreGame);

    SDL_Color colorText = GFX_UTIL_COLOR::White;
    if (_scoreGame < 0) {
        colorText = GFX_UTIL_COLOR::Red;
    }

    SDL_Rect rcs;
    rcs.x = tx - 2;
    rcs.w = tx + 190;
    rcs.y = ty - 2;
    rcs.h = ty + 46;
    // SDL_FillRect(_p_Screen, &rcs, SDL_MapRGBA(pScreen->format, 0, 0, 0, 0));
    // SDL 2
    SDL_FillSurfaceRect(_p_Screen, &rcs,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Screen->format),
                                   NULL, 0, 0, 0));

    LPErrInApp err =
        GFX_UTIL::DrawString(pScreen, buff, tx, ty, colorText, _p_FontBigText);
    if (err != NULL) {
        return err;
    }

    _scoreChanged = false;
    updateTextureAsFlipScreen();

    return err;
}

void SolitarioGfx::updateBadScoreScoreOnTime() {
    if (_p_currentTime->IsMoreThenOneSecElapsed()) {
        int deltaSec = _p_currentTime->GetDeltaFromLastUpdate();
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

void SolitarioGfx::clearScore() {
    int oldScore = _scoreGame;
    _scoreGame = 0;
    _scoreChanged = (oldScore != _scoreGame);
}

void SolitarioGfx::bonusScore() {
    int bonus = (2 * _scoreGame) - (_p_currentTime->GetNumOfSeconds() * 10);
    if (bonus > 0) {
        _scoreGame += bonus;
        _scoreChanged = true;
    }
}
