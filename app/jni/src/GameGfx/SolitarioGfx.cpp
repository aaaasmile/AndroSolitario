#include "SolitarioGfx.h"

#include <SDL_endian.h>
#include <SDL_image.h>
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

static const char *g_lpszSymbDir = DATA_PREFIX "images/";
extern const char *g_lpszDeckDir;

const int MYIDQUIT = 0;
const int MYIDNEWGAME = 1;

SolitarioGfx::SolitarioGfx() {
    _p_ScreenBackbufferDrag = 0;
    _startdrag = false;
    _p_Dragface = 0;
    _p_SceneBackground = 0;
    _p_ScreenTexture = 0;
    _p_BtQuit = NULL;
    _p_BtNewGame = NULL;
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
    }
    if (_p_BtQuit != NULL) {
        delete _p_BtQuit;
    }
    delete _p_currentTime;
}

void SolitarioGfx::clearSurface() {
    if (_p_ScreenBackbufferDrag != NULL) {
        SDL_FreeSurface(_p_ScreenBackbufferDrag);
        _p_ScreenBackbufferDrag = NULL;
    }
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
        _p_ScreenTexture = NULL;
    }
    if (_p_AlphaDisplay != NULL) {
        SDL_FreeSurface(_p_AlphaDisplay);
        _p_AlphaDisplay = NULL;
    }
}

// command buttons
void fncBind_ButtonQuitClick(void *self, int val) {
    SolitarioGfx *pApp = (SolitarioGfx *)self;
    pApp->BtQuitClick();
}

ClickCb SolitarioGfx::prepClickQuitCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonQuitClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

void fncBind_ButtonNewGameClick(void *self, int val) {
    SolitarioGfx *pApp = (SolitarioGfx *)self;
    pApp->BtNewGameClick();
}

ClickCb SolitarioGfx::prepClickNewGameCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonNewGameClick)};
    return (ClickCb){.tc = &tc, .self = this};
}

LPErrInApp SolitarioGfx::Initialize(
    SDL_Surface *s, SDL_Renderer *r, SDL_Window *w, DeckType &dt,
    LPLanguages planguages, TTF_Font *pfontSmallText, TTF_Font *pfontBigText,
    SDL_Surface *pSceneBackground, MusicManager *pMusicManager, bool isBlack,
    HighScore *pHighScore) {
    TRACE("Initialize Solitario\n");
    setDeckType(dt);
    _p_MusicManager = pMusicManager;
    _p_HighScore = pHighScore;
    _sceneBackgroundIsBlack = isBlack;
    _p_Languages = planguages;
    _p_FontBigText = pfontBigText;
    _p_FontSmallText = pfontSmallText;
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

    _p_AlphaDisplay = SDL_CreateRGBSurface(SDL_SWSURFACE, _p_Screen->w,
                                           _p_Screen->h, 32, 0, 0, 0, 0);
    if (_p_AlphaDisplay == NULL) {
        return ERR_UTIL::ErrorCreate("Cannot create alpha display: %s\n",
                                     SDL_GetError());
    }

    _p_ScreenBackbufferDrag = SDL_CreateRGBSurface(
        SDL_SWSURFACE, _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

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

LPErrInApp SolitarioGfx::DrawCardStack(SDL_Surface *s,
                                       LPCardRegionGfx pcardRegion) {
    if (pcardRegion == NULL) {
        return ERR_UTIL::ErrorCreate("DrawCardStack region is NULL");
    }
    // TRACE_DEBUG("Draw card stack %d\n", pcardRegion->RegionTypeId());

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
    CardRegionGfx *cr =
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

LPErrInApp SolitarioGfx::InitDrag(int x, int y, bool &isInitDrag,
                                  LPCardRegionGfx pSrcRegion) {
    return InitDrag(NULL, x, y, isInitDrag, pSrcRegion);
}

LPErrInApp SolitarioGfx::InitDrag(LPCardStackGfx pCargoStack, int x, int y,
                                  bool &isInitDrag,
                                  LPCardRegionGfx pSrcRegion) {
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

    DrawStaticScene();
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
        SDL_FreeSurface(_p_Dragface);
    }

    _p_Dragface = SDL_CreateRGBSurface(SDL_SWSURFACE, _dragPileInfo.width,
                                       _dragPileInfo.height, 32, 0, 0, 0, 0);
    SDL_FillRect(_p_Dragface, NULL, SDL_MapRGB(_p_Dragface->format, 0, 255, 0));
    SDL_SetColorKey(_p_Dragface, true,
                    SDL_MapRGB(_p_Dragface->format, 0, 255, 0));

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
    return NULL;
}

void SolitarioGfx::updateTextureAsFlipScreen() {
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);
    SDL_RenderCopy(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);
}

void SolitarioGfx::DoDrag(int x, int y) {
    // TRACE("DoDrag (x=%d, y=%d) => drag_x=%d, drag_y=%d. old_x=%d, old_y=%d",
    // x, y, _dragCard.x, _dragCard.y, _oldx, _oldy);
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

    if (_dragPileInfo.x == pCard->X() && _dragPileInfo.y == pCard->Y())
        return pBestRegion;  // when no movement

    zoomDropCard(_dragPileInfo.x, _dragPileInfo.y, pCard, _dragPileInfo.width,
                 _dragPileInfo.height);

    SDL_FreeSurface(_p_Dragface);
    _p_Dragface = NULL;
    return pBestRegion;
}

void calcPt(int x0, int y0, int x1, int y1, float t, int &xf, int &yf) {
    xf = int(x0 + t * (x1 - x0) + .5);
    yf = int(y0 + t * (y1 - y0) + .5);
}

void SolitarioGfx::zoomDropCard(int &sx, int &sy, LPCardGfx pCard, int w,
                                int h) {
    // TRACE("Zoom card x=%d, y=%d", sx, sy);
    SDL_Rect rcs;
    SDL_Rect rcd;
    SDL_Rect dest;

    int px, py;
    int dx = pCard->X();
    int dy = pCard->Y();
    float precision = 0.1f;

    for (float i = 0.0; i <= 1.0; i += precision) {
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
    SDL_FillRect(_p_Screen, &_p_Screen->clip_rect,
                 SDL_MapRGBA(_p_Screen->format, 0, 0, 0, 0));
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
    _scoreChanged = true;
    drawScore(_p_Screen);
    // it seems here that SDL_BlitSurface copy only the bitmap and not the fill
    // rect, do it also into the backbuffer
    SDL_FillRect(_p_ScreenBackbufferDrag, &_p_ScreenBackbufferDrag->clip_rect,
                 SDL_MapRGBA(_p_ScreenBackbufferDrag->format, 0, 0, 0, 0));
    SDL_BlitSurface(_p_Screen, NULL, _p_ScreenBackbufferDrag, NULL);

    updateTextureAsFlipScreen();
}

LPErrInApp SolitarioGfx::DrawInitialScene() {
    TRACE("DrawInitialScene\n");
    SDL_FillRect(_p_Screen, &_p_Screen->clip_rect,
                 SDL_MapRGBA(_p_Screen->format, 0, 0, 0, 0));
    SDL_Rect rctTarget;
    rctTarget.x = (_p_Screen->w - _p_SceneBackground->w) / 2;
    rctTarget.y = (_p_Screen->h - _p_SceneBackground->h) / 2;
    rctTarget.w = _p_SceneBackground->w;
    rctTarget.h = _p_SceneBackground->h;
    if (!_sceneBackgroundIsBlack) {
        fade(_p_Screen, _p_SceneBackground, 2, 0, _p_sdlRenderer, &rctTarget);
    }

    SDL_Rect rctBt1;
    int btw = 120;
    int bth = 28;
    int btoffsetY = 70;
    int btposx = 150;
    int btintraX = 30;
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    if (pGameSettings->NeedScreenMagnify()) {
        btw = 200;
        bth = 70;
        btoffsetY = 400;
        btposx = 500;
        btintraX = 50;
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

LPErrInApp SolitarioGfx::DrawCard(int x, int y, int nCdIndex, SDL_Surface *s) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Draw a card on NULL surface. This is wrong");
    }
    return DrawCardPac(x, y, nCdIndex, s);
}

LPErrInApp SolitarioGfx::DrawCardPac(int x, int y, int nCdIndex,
                                     SDL_Surface *s) {
    if (nCdIndex < 0 || nCdIndex >= _deckType.GetNumCards())
        return ERR_UTIL::ErrorCreate("DrawCardPac %d index out of range",
                                     nCdIndex);

    int suitIx = nCdIndex / _deckType.GetNumCardInSuit();
    int cardIx = nCdIndex % _deckType.GetNumCardInSuit();
    TRACE_DEBUG("Suit %d, card: %d\n", suitIx, cardIx);

    SDL_Rect srcCard;
    srcCard.x = suitIx * g_CardWidth;
    srcCard.y = cardIx * g_CardHeight;
    srcCard.w = g_CardWidth;
    srcCard.h = g_CardHeight;

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    if (SDL_BlitSurface(_p_Deck, &srcCard, s, &dest) == -1) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardPac with Iterator error: %s\n",
            SDL_GetError());
    }
    return NULL;
}

LPErrInApp SolitarioGfx::DrawCard(LPCardGfx pCard, SDL_Surface *s) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate("Error in draw card, surface is NULL\n");
    }
    // TRACE("Draw card ix = %d, suit = %s, rank %d, x,y %d,%d", pCard->Index(),
    //       pCard->SuitStr(), pCard->Rank(), pCard->X(), pCard->Y());
    return DrawCardPac(pCard, s);
}

LPErrInApp SolitarioGfx::DrawCardPac(LPCardGfx pCard, SDL_Surface *s) {
    pCard->SetDeckSurface(_p_Deck);

    return pCard->DrawCardPac(s);
}

LPErrInApp SolitarioGfx::DrawCardBack(int x, int y) {
    return DrawCardBack(x, y, _p_Screen);
}

LPErrInApp SolitarioGfx::DrawCardBack(int x, int y, SDL_Surface *s) {
    if (s == NULL) {
        return ERR_UTIL::ErrorCreate(
            "Error in DrawCardBack, surface is NULL\n");
    }

    return DrawCardBackPac(x, y, s);
}

LPErrInApp SolitarioGfx::DrawCardBackPac(int x, int y, SDL_Surface *s) {
    SDL_Rect dest, srcBack;
    dest.x = x;
    dest.y = y;

    srcBack.x = 0;
    srcBack.y = 0;
    srcBack.w = g_SymbolWidth;
    srcBack.h = g_SymbolHeight;

    if (SDL_BlitSurface(_p_Symbols, &srcBack, s, &dest) == -1) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardBackPac error: %s\n", SDL_GetError());
    }

    return NULL;
}

LPErrInApp SolitarioGfx::DrawSymbol(int x, int y, int nSymbol) {
    return DrawSymbol(x, y, nSymbol, _p_Screen);
}

LPErrInApp SolitarioGfx::DrawSymbol(int x, int y, int nSymbol, SDL_Surface *s) {
    if (nSymbol < 1) {
        return ERR_UTIL::ErrorCreate("Symbol index %d out of range", nSymbol);
    }
    if (nSymbol > 3)
        nSymbol = 3;

    return DrawSymbolPac(x, y, nSymbol, s);
}

LPErrInApp SolitarioGfx::DrawSymbolPac(int x, int y, int nSymbol,
                                       SDL_Surface *s) {
    SDL_Rect srcCard;
    srcCard.x = nSymbol * g_SymbolWidth;
    srcCard.y = 0;
    srcCard.w = g_SymbolWidth;
    srcCard.h = g_SymbolHeight;

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    if (SDL_BlitSurface(_p_Symbols, &srcCard, s, &dest) == -1) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawSymbolPac error: %s\n", SDL_GetError());
    }

    return NULL;
}

LPErrInApp SolitarioGfx::VictoryAnimation() {
    srand((unsigned)time(NULL));
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
                    case SDL_QUIT:
                        return NULL;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            return NULL;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        return NULL;
                    case SDL_FINGERDOWN:
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
        } while ((x + g_CardWidth > 0) && (x < _p_Screen->w));
    }
    return NULL;
}

LPErrInApp SolitarioGfx::LoadCardPac() {
    Uint16 w, h;
    GFX_UTIL::LoadCardPac(&_p_Deck, _deckType, &w, &h);
    TRACE("Pac size  w = %d, h = %d\n", w, h);
    g_CardWidth = w / 4;
    g_CardHeight = h / _deckType.GetNumCardInSuit();
    return NULL;
}

LPErrInApp SolitarioGfx::LoadSymbolsForPac() {
    std::string strFileSymbName = g_lpszSymbDir;
    strFileSymbName += _deckType.GetSymbolFileName();
    if (_deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
        SDL_RWops *srcSymb = SDL_RWFromFile(strFileSymbName.c_str(), "rb");
        if (srcSymb == NULL) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWFromFile on symbols failed: %s\n", SDL_GetError());
        }
        _p_Symbols = IMG_LoadPNG_RW(srcSymb);
        if (_p_Symbols == NULL) {
            return ERR_UTIL::ErrorCreate(
                "IMG_LoadPNG_RW on symbols file error (file %s): %s\n",
                strFileSymbName.c_str(), SDL_GetError());
        }
        SDL_SetColorKey(_p_Symbols, true,
                        SDL_MapRGB(_p_Symbols->format, 248, 0, 241));
    } else {
        _p_Symbols = SDL_LoadBMP(strFileSymbName.c_str());
        if (_p_Symbols == 0) {
            return ERR_UTIL::ErrorCreate("Load bitmap failed: %s\n",
                                         SDL_GetError());
        }

        if (_deckType.GetSymbolFileName() == "symb_336.bmp") {
            SDL_SetColorKey(_p_Symbols, true,
                            SDL_MapRGB(_p_Symbols->format, 242, 30, 206));
        } else {
            SDL_SetColorKey(_p_Symbols, true,
                            SDL_MapRGB(_p_Symbols->format, 0, 128, 0));
        }
    }

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

LPErrInApp SolitarioGfx::handleGameLoopKeyDownEvent(SDL_Event &event) {
    LPErrInApp err;
    if (event.key.keysym.sym == SDLK_n) {
        _newgamerequest = true;
    }
    if (event.key.keysym.sym == SDLK_a) {
        err = VictoryAnimation();
        if (err != NULL) {
            return err;
        }
        DrawStaticScene();
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopFingerDownEvent(SDL_Event &event) {
    _p_BtQuit->FingerDown(event);
    _p_BtNewGame->FingerDown(event);
    return NULL;
}

LPErrInApp SolitarioGfx::handleGameLoopMouseDownEvent(SDL_Event &event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        return handleLeftMouseDown(event);
    } else if (event.button.button == SDL_BUTTON_RIGHT) {
        return handleRightMouseDown(event);
    }
    return NULL;
}

LPErrInApp SolitarioGfx::handleLeftMouseDown(SDL_Event &event) {
    LPErrInApp err;
    CardRegionGfx *srcReg;
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
            SDL_ShowCursor(SDL_DISABLE);
            SDL_SetWindowGrab(_p_Window, SDL_TRUE);
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

LPErrInApp SolitarioGfx::handleRightMouseDown(SDL_Event &event) {
    LPErrInApp err;
    CardRegionGfx *srcReg;
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

void SolitarioGfx::handleGameLoopMouseMoveEvent(SDL_Event &event) {
    if (event.motion.state == SDL_BUTTON(1) && _startdrag) {
        DoDrag(event.motion.x, event.motion.y);
    }
    bool statusChanged = _p_BtNewGame->MouseMove(event);
    statusChanged = statusChanged || _p_BtQuit->MouseMove(event);
    if (statusChanged) {
        DrawStaticScene();
    }
}

LPErrInApp SolitarioGfx::handleGameLoopMouseUpEvent(SDL_Event &event) {
    _p_BtQuit->MouseUp(event);
    _p_BtNewGame->MouseUp(event);

    if (_startdrag) {
        _startdrag = false;
        LPCardRegionGfx pDestReg = DoDrop();
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetWindowGrab(_p_Window, SDL_FALSE);
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
        _p_currentTime->StopTimer();
        bonusScore();
        DrawStaticScene();
        char buff[1024];
        snprintf(buff, 1024, _p_Languages->GetCStringId(Languages::FINAL_SCORE),
                 _scoreGame);
        showOkMsgBox(buff);
        err = _p_HighScore->SaveScore(_scoreGame, _deckType.GetNumCards());
        if (err != NULL) {
            return err;
        }
        DrawStaticScene();
        VictoryAnimation();
        err = newGame();
        if (err != NULL) {
            return err;
        }
        DrawStaticScene();
    }

    return NULL;
}

LPErrInApp SolitarioGfx::StartGameLoop() {
    _p_MusicManager->PlayMusic(MusicManager::MUSIC_PLAY_SND,
                               MusicManager::eLoopType::LOOP_ON);
    // button Quit
    STRING strTextBt;
    strTextBt = _p_Languages->GetStringId(Languages::ID_EXIT);
    _p_BtQuit->SetButtonText(strTextBt.c_str());
    _p_BtQuit->SetVisibleState(ButtonGfx::VISIBLE);
    // button New Game
    strTextBt = _p_Languages->GetStringId(Languages::ID_NEWGAME);
    _p_BtNewGame->SetButtonText(strTextBt.c_str());
    _p_BtNewGame->SetVisibleState(ButtonGfx::VISIBLE);

    // index 0 (deck with face down)
    CreateRegion(RT_DECKSTOCK,          // ID
                 CRD_VISIBLE | CRD_3D,  // attributes
                 CRD_DONOTHING,         // Accept mode
                 CRD_DONOTHING,         // drag mode
                 CRD_OSYMBOL,           // symbol
                 35, 10, 2, 2);         // x, y, x offset, yoffset
    // index 1-7
    int i;
    for (i = 1; i <= 7; i++) {
        CreateRegion(RegionType::RT_TABLEAU,
                     CRD_VISIBLE | CRD_DODRAG | CRD_DODROP,  // attributes
                     CRD_DOOPCOLOR | CRD_DOLOWER | CRD_DOLOWERBY1 |
                         CRD_DOKING,  // accept mode
                     CRD_DRAGFACEUP,  // drag mode
                     CRD_HSYMBOL,     // symbol
                     (g_CardWidth * (i - 1)) + (i * 17), g_CardHeight + 40, 0,
                     32);  // x, y, x offset, yoffset
    }

    // index 8 (deck face up)
    CreateRegion(RegionType::RT_DECKSTOCK_FACEUP,
                 CRD_VISIBLE | CRD_FACEUP | CRD_DODRAG | CRD_3D,  // Attributes
                 CRD_DOALL,                                       // accept mode
                 CRD_DRAGTOP,                                     // drag mode
                 CRD_NSYMBOL,                                     // symbol
                 g_CardWidth + 65, 10, 0, 0);  // x, y, x offset, yoffset

    // index 9-12 (4 aces place on the top)
    for (i = 4; i <= 7; i++) {
        CreateRegion(
            RegionType::RT_ACE_FOUNDATION,
            CRD_VISIBLE | CRD_3D | CRD_DODRAG | CRD_DODROP,  // Attributes
            CRD_DOSINGLE | CRD_DOHIGHER | CRD_DOHIGHERBY1 | CRD_DOACE |
                CRD_DOSUIT,  // Accept mode
            CRD_DRAGTOP,     // drop mode
            CRD_HSYMBOL,     // symbol
            (g_CardWidth * (i - 1)) + (i * 17), 10, 0,
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
                case SDL_QUIT:
                    fade(_p_Screen, _p_Screen, 1, 1, _p_sdlRenderer, NULL);
                    return NULL;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        return NULL;
                    }
                    err = handleGameLoopKeyDownEvent(event);
                    if (err != NULL)
                        return err;
                    break;
                case SDL_FINGERDOWN:
                    err = handleGameLoopFingerDownEvent(event);
                    if (err != NULL)
                        return err;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    err = handleGameLoopMouseDownEvent(event);
                    if (err != NULL)
                        return err;
                    break;

                case SDL_MOUSEMOTION:
                    handleGameLoopMouseMoveEvent(event);
                    break;

                case SDL_MOUSEBUTTONUP:
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
    LPGameSettings pGameSettings = GAMESET::GetSettings();
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
    SDL_FillRect(_p_AlphaDisplay, &_p_AlphaDisplay->clip_rect,
                 SDL_MapRGBA(_p_AlphaDisplay->format, 0, 0, 0, 0));
    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    STRING strTextYes = _p_Languages->GetStringId(Languages::ID_YES);
    STRING strTextNo = _p_Languages->GetStringId(Languages::ID_NO);

    return MsgBox.Show(_p_AlphaDisplay, strTextYes.c_str(), strTextNo.c_str(),
                       strText);
}

void SolitarioGfx::showOkMsgBox(LPCSTR strText) {
    LPGameSettings pGameSettings = GAMESET::GetSettings();
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
    SDL_FillRect(_p_AlphaDisplay, &_p_AlphaDisplay->clip_rect,
                 SDL_MapRGBA(_p_AlphaDisplay->format, 0, 0, 0, 0));
    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    STRING strTextOk = _p_Languages->GetStringId(Languages::ID_OK);
    MsgBox.Show(_p_AlphaDisplay, strTextOk.c_str(), "", strText);
}

void SolitarioGfx::BtQuitClick() {
    TRACE("Quit with user button\n");
    if (showYesNoMsgBox(_p_Languages->GetCStringId(Languages::ASK_QUIT)) ==
        MesgBoxGfx::RES_YES) {
        _terminated = true;
    } else {
        DrawStaticScene();
    }
}

void SolitarioGfx::BtNewGameClick() {
    TRACE("New Game with user button\n");
    if (showYesNoMsgBox(_p_Languages->GetCStringId(Languages::ASK_NEWGAME)) ==
        MesgBoxGfx::RES_YES) {
        _newgamerequest = true;
    } else {
        DrawStaticScene();
    }
}

LPErrInApp SolitarioGfx::drawScore(SDL_Surface *pScreen) {
    if (!_scoreChanged) {
        return NULL;
    }
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    int tx = 10;
    int offsetY = 30;
    if (pGameSettings->NeedScreenMagnify()) {
        tx = 100;
        offsetY = 400;
    }
    int ty = pScreen->h - offsetY;
    char buff[256];
    snprintf(buff, sizeof(buff), "%s : %d",
             _p_Languages->GetCStringId(Languages::ID_SCORE), _scoreGame);

    SDL_Color colorText = GFX_UTIL_COLOR::White;
    if (_scoreGame < 0) {
        colorText = GFX_UTIL_COLOR::Red;
    }

    SDL_Rect rcs;
    rcs.x = tx - 2;
    rcs.w = tx + 190;
    rcs.y = ty - 2;
    rcs.h = ty + 46;
    SDL_FillRect(_p_Screen, &rcs, SDL_MapRGBA(pScreen->format, 0, 0, 0, 0));

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
