#include "InvidoGfx.h"

#include <SDL3/SDL_endian.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <time.h>

#include "BalloonGfx.h"
#include "ButtonGfx.h"
#include "DeckType.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "InvidoCoreEnv.h"
#include "Languages.h"
#include "MesgBoxGfx.h"
#include "MusicManager.h"
#include "PopUpMenuGfx.h"
#include "DeckLoader.h"

static const char* lpszImageDir = DATA_PREFIX "images/invido/";
static const char* lpszImageBack = "im000740.jpg";
static const char* lpszaImage_filenames[] = {
    DATA_PREFIX "images/invido/tocca.png",
    DATA_PREFIX "images/invido/LedOff.bmp",
    DATA_PREFIX "images/invido/LedOn.bmp",
    DATA_PREFIX "images/invido/canela.bmp",
    DATA_PREFIX "images/invido/uovo.bmp",
    DATA_PREFIX "images/invido/vertical.png",
    DATA_PREFIX "images/invido/cornerdl.png",
    DATA_PREFIX "images/invido/cornerdr.png",
    DATA_PREFIX "images/invido/cornerul.png",
    DATA_PREFIX "images/invido/cornerur.png",
    DATA_PREFIX "images/invido/horizontal.png",
    DATA_PREFIX "images/invido/LedOnBlue.bmp",
    DATA_PREFIX "images/invido/LedOnRed.bmp",
    DATA_PREFIX "images/invido/canela_obl.bmp",
    DATA_PREFIX "images/invido/balloon_body.pcx",
    DATA_PREFIX "images/invido/balloon_up.pcx"};

static const char* lpszCST_INFO = "[INFO]";
static const char* lpszCST_SCORE = "[SCORE]";
static const char* lpszCST_SU = "[SU]";

static void fncBind_ButtonClicked(void* self, int iVal) {
    InvidoGfx* pInvidoGfx = (InvidoGfx*)self;
    pInvidoGfx->ButCmdClicked(iVal);
}


InvidoGfx::InvidoGfx() {
    _p_Scene_background = 0;
    _p_FontText = 0;
    _p_Surf_Bar = 0;

    _p_Deck = 0;
    _p_Symbols = 0;
    _p_FontStatus = 0;

    for (int i = 0; i < InvidoGfx::NUM_ANIMAGES; i++) {
        _p_AnImages[i] = 0;
    }
    _p_MatchPoints = 0;
    _p_InvidoCore = 0;
    _isPlayerCanPlay = false;
    _playerThatHaveMarkup = 0;
    for (int j = 0; j < InvidoGfx::NUMOFBUTTON; j++) {
        _p_btArrayCmd[j] = 0;
    }
    _p_MusicMgr = 0;
    _isMatchTerminated = false;
    _p_AlphaDisplay = 0;
    _p_DeckLoader = new DeckLoader();
}

InvidoGfx::~InvidoGfx() { cleanup(); }


LPErrInApp InvidoGfx::Initialize(SDL_Surface* pScreen,
                                 UpdateScreenCb& fnUpdateScreen,
                                 SDL_Window* pWindow,
                                 SDL_Surface* pSceneBackground,
                                 UpdateHighScoreCb& fnUpdateHighScore) {
    TRACE("Initialize Invido\n");
    LPErrInApp err = NULL;
    _fnUpdateScreen = fnUpdateScreen;
    _p_Screen = pScreen;
    _p_Window = pWindow;

    cleanup();

    GameSettings* pGameSettings = GameSettings::GetSettings();
    setDeckType(pGameSettings->DeckTypeVal);
   
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

    _p_Scene_background = pSceneBackground;

    createRegionsInit();

    _p_FontStatus = GameSettings::GetSettings()->GetFontDjvBoldBig();
    _p_FontText = pGameSettings->GetFontDjvSmall();

    // load images for animation stuff
    for (int i = 0; i < NUM_ANIMAGES; i++) {
        _p_AnImages[i] = IMG_Load(lpszaImage_filenames[i]);
    }

    _p_MusicMgr = pGameSettings->GetMusicManager();

    // command buttons
    SDL_Rect rctBt;
    rctBt.w = 120;
    rctBt.h = 28;
    rctBt.y = _p_Screen->h - 155 - rctBt.h - 20;
    int iXButInit = _p_Screen->w - rctBt.w - 20;

    ClickCb cbBtClicked = prepClickCb();
    for (int i = 0; i < InvidoGfx::NUMOFBUTTON; i++) {
        rctBt.x = iXButInit - i * (rctBt.w + 10);
        _p_btArrayCmd[i] = new ButtonGfx;
        _p_btArrayCmd[i]->Initialize(&rctBt, _p_Screen, _p_FontStatus, i,
                                     cbBtClicked);
    }

    // balloon
    SDL_Rect rctBall;
    rctBall.x = 320;
    rctBall.y = 100;
    rctBall.w = _p_AnImages[IMG_BALLOON]->w;
    rctBall.h = _p_AnImages[IMG_BALLOON]->h;
    _p_balGfx = new BalloonGfx();
    _p_balGfx->Initialize(&rctBall, _p_AnImages[IMG_BALLOON], _p_FontStatus,
                          200);
    _p_balGfx->SetStyle(BalloonGfx::ARROW_UP, _p_AnImages[IMG_BALL_ARROW_UP]);

    // messagebox background surface
    _p_AlphaDisplay =
        SDL_CreateSurface(_p_Screen->w, _p_Screen->h, SDL_PIXELFORMAT_XRGB8888);

    return NULL;
}

void InvidoGfx::setMapValues(){
    GameSettings* pGameSettings = GameSettings::GetSettings();
    Languages* pLangMgr = pGameSettings->GetLanguageMan();
    // points name
    _MapPunti[SC_PATTA] = pLangMgr->GetStringId(Languages::ID_S_PATA);
    _MapPunti[SC_CANELA] = pLangMgr->GetStringId(Languages::ID_S_CANELA);
    _MapPunti[SC_INVIDO] = pLangMgr->GetStringId(Languages::ID_S_INVIDO);
    _MapPunti[SC_TRASMAS] = pLangMgr->GetStringId(Languages::ID_S_TRASMAS);
    _MapPunti[SC_TRASMASNOEF] =
        pLangMgr->GetStringId(Languages::ID_S_TRASMASNOEF);
    _MapPunti[SC_FUERAJEUQ] = pLangMgr->GetStringId(Languages::ID_S_FUERAJEUQ);
    _MapPunti[SC_PARTIDA] = pLangMgr->GetStringId(Languages::ID_S_PARTIDA);

    // buttons  strings
    _Map_bt_Say[eSayPlayer::SP_AMONTE] =
        pLangMgr->GetStringId(Languages::ID_S_BT_AMONTE);
    _Map_bt_Say[eSayPlayer::SP_INVIDO] =
        pLangMgr->GetStringId(Languages::ID_S_BT_INVIDO);
    _Map_bt_Say[eSayPlayer::SP_TRASMAS] =
        pLangMgr->GetStringId(Languages::ID_S_BT_TRASMAS);
    _Map_bt_Say[eSayPlayer::SP_TRASMASNOEF] =
        pLangMgr->GetStringId(Languages::ID_S_BT_TRASMASNOEF);
    _Map_bt_Say[eSayPlayer::SP_FUERAJEUQ] =
        pLangMgr->GetStringId(Languages::ID_S_BT_FUERAJEUQ);
    _Map_bt_Say[eSayPlayer::SP_PARTIDA] =
        pLangMgr->GetStringId(Languages::ID_S_BT_PARTIDA);
    _Map_bt_Say[eSayPlayer::SP_VABENE] =
        pLangMgr->GetStringId(Languages::ID_S_BT_VABENE);
    _Map_bt_Say[eSayPlayer::SP_VADOVIA] =
        pLangMgr->GetStringId(Languages::ID_S_BT_VADOVIA);
    _Map_bt_Say[eSayPlayer::SP_CHIAMADIPIU] =
        pLangMgr->GetStringId(Languages::ID_S_BT_CHIAMADIPIU);
    _Map_bt_Say[eSayPlayer::SP_NO] =
        pLangMgr->GetStringId(Languages::ID_S_BT_NO);
    _Map_bt_Say[eSayPlayer::SP_GIOCA] =
        pLangMgr->GetStringId(Languages::ID_S_BT_GIOCA);
    _Map_bt_Say[eSayPlayer::SP_VADODENTRO] =
        pLangMgr->GetStringId(Languages::ID_S_BT_VADODENTRO);
    _Map_bt_Say[eSayPlayer::SP_CHIAMA_BORTOLO] =
        pLangMgr->GetStringId(Languages::ID_S_BT_CHIAMA_BORTOLO);
    _Map_bt_Say[eSayPlayer::SP_NOTHING] = "";

    // say strings
    _Map_fb_Say[eSayPlayer::SP_AMONTE] =
        pLangMgr->GetStringId(Languages::ID_S_AMONTE);
    _Map_fb_Say[eSayPlayer::SP_INVIDO] =
        pLangMgr->GetStringId(Languages::ID_S_INVIDO);
    _Map_fb_Say[eSayPlayer::SP_TRASMAS] =
        pLangMgr->GetStringId(Languages::ID_S_TRASMAS);
    _Map_fb_Say[eSayPlayer::SP_TRASMASNOEF] =
        pLangMgr->GetStringId(Languages::ID_S_TRASMASNOEF);
    _Map_fb_Say[eSayPlayer::SP_FUERAJEUQ] =
        pLangMgr->GetStringId(Languages::ID_S_FUERAJEUQ);
    _Map_fb_Say[eSayPlayer::SP_PARTIDA] =
        pLangMgr->GetStringId(Languages::ID_S_PARTIDA);
    _Map_fb_Say[eSayPlayer::SP_VABENE] =
        pLangMgr->GetStringId(Languages::ID_S_VABENE);
    _Map_fb_Say[eSayPlayer::SP_VADOVIA] =
        pLangMgr->GetStringId(Languages::ID_S_VADOVIA);
    _Map_fb_Say[eSayPlayer::SP_CHIAMADIPIU] =
        pLangMgr->GetStringId(Languages::ID_S_CHIAMADIPIU);
    _Map_fb_Say[eSayPlayer::SP_NO] = pLangMgr->GetStringId(Languages::ID_S_NO);
    _Map_fb_Say[eSayPlayer::SP_GIOCA] =
        pLangMgr->GetStringId(Languages::ID_S_GIOCA);
    _Map_fb_Say[eSayPlayer::SP_VADODENTRO] =
        pLangMgr->GetStringId(Languages::ID_S_VADODENTRO);

    // sound call echo player
    _Map_id_EchoSay[eSayPlayer::SP_AMONTE] = MusicManager::SND_IG_MONTE_NORM;
    _Map_id_EchoSay[eSayPlayer::SP_INVIDO] = MusicManager::SND_IG_INV_NORM;
    _Map_id_EchoSay[eSayPlayer::SP_TRASMAS] = MusicManager::SND_IG_TRASMAS;
    _Map_id_EchoSay[eSayPlayer::SP_TRASMASNOEF] = MusicManager::SND_IG_TRASNOEF;
    _Map_id_EchoSay[eSayPlayer::SP_FUERAJEUQ] = MusicManager::SND_IG_FUORIGIOCO;
    _Map_id_EchoSay[eSayPlayer::SP_PARTIDA] = MusicManager::SND_IG_PARTIDA;
    _Map_id_EchoSay[eSayPlayer::SP_VABENE] = MusicManager::SND_IG_VABENE;
    _Map_id_EchoSay[eSayPlayer::SP_VADOVIA] = MusicManager::SND_IG_VUVIA;
    _Map_id_EchoSay[eSayPlayer::SP_CHIAMADIPIU] =
        MusicManager::SND_IG_CHIAMAPIU;
    _Map_id_EchoSay[eSayPlayer::SP_NO] = MusicManager::SND_IG_NO;
    _Map_id_EchoSay[eSayPlayer::SP_GIOCA] = MusicManager::SND_IG_GIOCA;
    _Map_id_EchoSay[eSayPlayer::SP_CHIAMA_BORTOLO] =
        MusicManager::SND_IG_BORTOLO;
    // sound synth opponent
    _Map_idSynth_Say[eSayPlayer::SP_AMONTE] = MusicManager::SND_WAV_SYF_MONTE;
    _Map_idSynth_Say[eSayPlayer::SP_INVIDO] = MusicManager::SND_WAV_SYF_INVIDO;
    _Map_idSynth_Say[eSayPlayer::SP_TRASMAS] =
        MusicManager::SND_WAV_SYF_TRASMAS;
    _Map_idSynth_Say[eSayPlayer::SP_TRASMASNOEF] =
        MusicManager::SND_WAV_SYF_NOEF;
    _Map_idSynth_Say[eSayPlayer::SP_FUERAJEUQ] =
        MusicManager::SND_WAV_SYF_FUORIGI;
    _Map_idSynth_Say[eSayPlayer::SP_PARTIDA] =
        MusicManager::SND_WAV_SYF_PARTIDA;
    _Map_idSynth_Say[eSayPlayer::SP_VABENE] = MusicManager::SND_WAV_SYF_VABENE;
    _Map_idSynth_Say[eSayPlayer::SP_VADOVIA] = MusicManager::SND_WAV_SYF_VUVIA;
    _Map_idSynth_Say[eSayPlayer::SP_CHIAMADIPIU] =
        MusicManager::SND_WAV_SYF_CHIADIPIU;

    _Map_idSynth_Say[eSayPlayer::SP_NO] = MusicManager::SND_WAV_SYF_NO;
    _Map_idSynth_Say[eSayPlayer::SP_GIOCA] = MusicManager::SND_WAV_SYF_GIOCA;
}


ClickCb InvidoGfx::prepClickCb() {
    static VClickCb const tc = {.Click = (&fncBind_ButtonClicked)};
    return (ClickCb){.tc = &tc, .self = this};
}

LPErrInApp InvidoGfx::OnResize(SDL_Surface* pScreen) { return NULL; }

LPErrInApp InvidoGfx::HandleEvent(SDL_Event* pEvent,
                                  const SDL_Point& targetPos) {
    return NULL;
}
LPErrInApp InvidoGfx::HandleIterate(bool& done) { return NULL; }

LPErrInApp InvidoGfx::Show() { return NULL; }

void InvidoGfx::cleanup() {
    if (_p_Scene_background) {
        SDL_DestroySurface(_p_Scene_background);
        _p_Scene_background = NULL;
    }

    if (_p_Surf_Bar) {
        SDL_DestroySurface(_p_Surf_Bar);
        _p_Surf_Bar = NULL;
    }
    if (_p_DeckLoader) {
        delete _p_DeckLoader;
        _p_DeckLoader = NULL;
    }
    if (_p_AlphaDisplay) {
        SDL_DestroySurface(_p_AlphaDisplay);
        _p_AlphaDisplay = NULL;
    }
    for (int i = 0; i < InvidoGfx::NUM_ANIMAGES; i++) {
        if (_p_AnImages[i] != NULL) {
            SDL_DestroySurface(_p_AnImages[i]);
            _p_AnImages[i] = NULL;
        }
    }
}

void InvidoGfx::drawStaticScene() {
    if (_p_Scene_background) {
        SDL_BlitSurface(_p_Scene_background, NULL, _p_Screen, NULL);
    }

    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        renderCard(&_aOpponentCards[i]);
        renderCard(&_aPlayerCards[i]);
    }
    for (int k = 0; k < NUM_CARDS_PLAYED; k++) {
        renderCard(&_CardsTable[k]);
    }

    showPlayerMarkup(_playerThatHaveMarkup);

    // shows names
    renderPlayerName(PLAYER1);
    renderPlayerName(PLAYER2);

    // show score (leds and points)
    showCurrentScore();

    // draw command buttons
    for (int j = 0; j < NUMOFBUTTON; j++) {
        _p_btArrayCmd[j]->DrawButton(_p_Screen);
    }

    // ballon
    _p_balGfx->Draw(_p_Screen);

    updateTextureAsFlipScreen();
}

void InvidoGfx::updateTextureAsFlipScreen() {
    (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_Screen);
}

void InvidoGfx::renderCard(CardGfx* pCard) {
    if (pCard->State == CardGfx::CSW_ST_INVISIBLE) {
        return;
    } else if (pCard->State == CardGfx::CSW_ST_SYMBOL) {
        pCard->DrawSymbol(_p_Screen);
    } else if (pCard->State == CardGfx::CSW_ST_VISIBLE) {
        pCard->DrawCard(_p_Screen);
    } else if (pCard->State == CardGfx::CSW_ST_BACK) {
        pCard->DrawCardBack(_p_Screen);
    } else {
        SDL_assert(0);
    }
}

void InvidoGfx::renderPlayerName(int iPlayerIx) {
    Player* pPlayer = _p_InvidoCore->GetPlayer(iPlayerIx);

    char txt_to_render[256];

    if (iPlayerIx == PLAYER2) {
        sprintf(txt_to_render, "%s", pPlayer->GetName());
        GFX_UTIL::DrawStaticSpriteEx(_p_Screen, 0, 0, 150, 25, 310, 17,
                                     _p_Surf_Bar);
        GFX_UTIL::DrawString(_p_Screen, txt_to_render, 315, 21,
                             GFX_UTIL_COLOR::White, _p_FontText);
    } else if (iPlayerIx == PLAYER1) {
        sprintf(txt_to_render, "%s", pPlayer->GetName());
        GFX_UTIL::DrawStaticSpriteEx(_p_Screen, 0, 0, 150, 25, 310,
                                     _p_Screen->h - 35, _p_Surf_Bar);
        GFX_UTIL::DrawString(_p_Screen, txt_to_render, 315, _p_Screen->h - 31,
                             GFX_UTIL_COLOR::White, _p_FontText);
    } else {
        SDL_assert(0);
    }
}

void InvidoGfx::createRegionsInit() {
    // opponent cards
    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        _aOpponentCards[i]._x = (_cardWidth * i) + ((i + 1) * 17);
        _aOpponentCards[i]._y = 10;
        _aOpponentCards[i].SetDeckSurface(_p_Deck);
        _aOpponentCards[i].SetWidth(_cardWidth);
        _aOpponentCards[i].SetHeight(_cardHeight);
        _aOpponentCards[i].SetSymbSurf(_p_Symbols, _symbolWidth, _symbolHeigth);
    }

    // player cards
    for (int k = 0; k < NUM_CARDS_HAND; k++) {
        _aPlayerCards[k]._x = (_cardWidth * k) + ((k + 1) * 17);
        _aPlayerCards[k]._y = _cardHeight * 3 + 10;
        _aPlayerCards[k].SetWidth(_cardWidth);
        _aPlayerCards[k].SetHeight(_cardHeight);
        _aPlayerCards[k].SetDeckSurface(_p_Deck);
        _aPlayerCards[k].SetSymbSurf(_p_Symbols, _symbolWidth, _symbolHeigth);
    }

    // cards played
    for (int g = 0; g < NUM_CARDS_PLAYED; g++) {
        _CardsTable[g]._x = (_cardWidth * (2 - 1)) + ((2 + g * 3) * 17);
        _CardsTable[g]._y = _cardHeight * 2 - _cardHeight / 2 + 10;
        _CardsTable[g].State = CardGfx::CSW_ST_INVISIBLE;
        _CardsTable[g].SetDeckSurface(_p_Deck);
        _CardsTable[g].SetWidth(_cardWidth);
        _CardsTable[g].SetHeight(_cardHeight);
        _CardsTable[g].SetSymbSurf(_p_Symbols, _symbolWidth, _symbolHeigth);
    }
}

void InvidoGfx::animateBeginGiocata() {
    TRACE_DEBUG("animateBeginGiocata - begin\n");
    Uint32 uiTickTot = 0;
    Uint64 uiInitialTick = SDL_GetTicks();
    Uint64 uiLast_time = uiInitialTick;
    CardGfx cardTmp[NUM_CARDS_HAND];
    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        cardTmp[i].Copy(&_aPlayerCards[i]);
        cardTmp[i]._y = 40;
        cardTmp[i].SetDeckSurface(_p_Deck);
    }
    cardTmp[0]._x = 10;
    cardTmp[1]._x = 10;
    cardTmp[2]._x = 10;
    int yspeed = 0;
    int xspeed = 0;
    int GRAVITY = 1;
    do {
        // clear screen
        SDL_BlitSurface(_p_Scene_background, NULL, _p_Screen, NULL);

        for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
            if (iManoNum == 0) {
                if (cardTmp[iManoNum]._x >
                    _aPlayerCards[NUM_CARDS_HAND - 1]._x +
                        _aPlayerCards[NUM_CARDS_HAND - 1].Width()) {
                    // go back on x
                    xspeed = -6;
                }
                yspeed = yspeed + GRAVITY;
                xspeed = xspeed + GRAVITY;
            }
            cardTmp[iManoNum]._x += xspeed;
            cardTmp[iManoNum]._y += yspeed;

            if (cardTmp[iManoNum]._y >= _aPlayerCards[iManoNum]._y) {
                // ok the card reach the position
                cardTmp[iManoNum]._y = _aPlayerCards[iManoNum]._y;
                cardTmp[iManoNum]._x = _aPlayerCards[iManoNum]._x;
                // uiTickTot
            }

            // update card position
            cardTmp[iManoNum].DrawCard(_p_Screen);
        }
        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                          _p_Screen->pitch);  // sdl 2.0
        SDL_RenderClear(_p_sdlRenderer);
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);

        // synch to frame rate
        Uint64 uiNowTime = SDL_GetTicks();
        uiTickTot = (Uint32)(uiNowTime - uiInitialTick);
        if (uiNowTime < uiLast_time + (1000 / FPS)) {
            SDL_Delay((Uint32)(uiLast_time + (1000 / FPS) - uiNowTime));
            uiLast_time = uiNowTime;
        }
    } while (uiTickTot < 1500);

    TRACE_DEBUG("animateBeginGiocata - end\n");

    // restore previous scene
    drawStaticScene();
}

void InvidoGfx::animateManoEnd(int iPlayerIx) {
    Uint32 uiTickTot = 0;
    Uint64 uiInitialTick = SDL_GetTicks();
    Uint64 uiLast_time = uiInitialTick;
    Uint32 uiFrameRate = 3;
    CardGfx cardTmp[NUM_CARDS_PLAYED];
    for (int i = 0; i < NUM_CARDS_PLAYED; i++) {
        cardTmp[i].Copy(&_CardsTable[i]);
        cardTmp[i].SetDeckSurface(_p_Deck);
    }

    int iPhase1Speed = 2;
    int iPhase2Speed = 1;

    // switch (g_Options.All.iAniSpeedLevel) { // Options logic seems missing or
    // changed
    //     case 0: iPhase1Speed = 1; iPhase2Speed = 1; break;
    //     case 1: iPhase1Speed = 2; iPhase2Speed = 2; break;
    //     case 2: iPhase1Speed = 3; iPhase2Speed = 3; break;
    //     case 3: iPhase1Speed = 5; iPhase2Speed = 5; break;
    //     case 4: iPhase1Speed = 7; iPhase2Speed = 7; break;
    //     case 5: iPhase1Speed = 10; iPhase2Speed = 10; break;
    //     default: iPhase1Speed = 2; iPhase2Speed = 1; break;
    // }

    // move on x
    cardTmp[0]._vy = 0;
    cardTmp[0]._vx = +iPhase1Speed;
    cardTmp[1]._vy = 0;
    cardTmp[1]._vx = -iPhase1Speed;

    // freeze the current display used as background
    SDL_Surface* pCurrentDisplay = GFX_UTIL::SDL_CreateRGBSurface(
        _p_Screen->w, _p_Screen->h, 32, 0, 0, 0, 0);

    SDL_BlitSurface(_p_Screen, NULL, pCurrentDisplay, NULL);

    bool bEnd = false;
    bool bPhase1_X = false;
    int loopCount = 0;
    do {
        // clear screen
        SDL_BlitSurface(pCurrentDisplay, NULL, _p_AlphaDisplay, NULL);

        for (int iCardPlayedIndex = 0; iCardPlayedIndex < NUM_CARDS_PLAYED;
             iCardPlayedIndex++) {
            cardTmp[iCardPlayedIndex]._x += cardTmp[iCardPlayedIndex]._vx;
            cardTmp[iCardPlayedIndex]._y += cardTmp[iCardPlayedIndex]._vy;

            if (cardTmp[iCardPlayedIndex].State == CardGfx::CSW_ST_VISIBLE) {
                // update card position
                cardTmp[iCardPlayedIndex].DrawCard(_p_AlphaDisplay);
            }
        }
        if (!bPhase1_X && cardTmp[1]._x <= cardTmp[0]._x) {
            // ok the card reach the central position
            cardTmp[0]._vx = 0;
            cardTmp[1]._vx = 0;
            bPhase1_X = true;
        }

        SDL_BlitSurface(_p_AlphaDisplay, NULL, _p_Screen, NULL);
        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                          _p_Screen->pitch);  // sdl 2.0
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);

        int iIncVel = iPhase2Speed;

        if (bPhase1_X) {
            // second step, move cards to the trick winner
            if (iPlayerIx == 0) {
                cardTmp[0]._vy += iIncVel;
                cardTmp[1]._vy += iIncVel;
                if (cardTmp[1]._y >= _p_Screen->h) {
                    // cards outside of the screen
                    bEnd = true;
                }
            } else if (iPlayerIx == 1) {
                cardTmp[0]._vy -= iIncVel;
                cardTmp[1]._vy -= iIncVel;
                if (cardTmp[0]._y <= 0) {
                    // cards outside of the screen
                    bEnd = true;
                }
            } else {
                // patada
                cardTmp[0]._vx -= iIncVel;
                cardTmp[1]._vx -= iIncVel;
                if (cardTmp[1]._x <= 0) {
                    // cards outside of the screen
                    bEnd = true;
                }
            }
        }

        // synch to frame rate
        Uint64 uiNowTime = SDL_GetTicks();
        uiTickTot = (Uint32)(uiNowTime - uiInitialTick);
        if (uiNowTime < uiLast_time + uiFrameRate) {
            SDL_Delay((Uint32)(uiLast_time + uiFrameRate - uiNowTime));
            uiLast_time = uiNowTime;
        }
        loopCount += 1;
        if (loopCount > 1000) {
            bEnd = true;
        }
    } while (!bEnd);

    SDL_DestroySurface(pCurrentDisplay);
}

void InvidoGfx::animGiocataEnd(int iPlayerIx, bool bIsPata) {
    int iTickTot = 0;
    int iTickFlashDiff = 0;
    SDL_Rect destWIN;
    SDL_Rect destLOS;
    bool bFlash = true;
    int iCooYA = _p_Screen->h - 30;
    int iCooYB = 20;

    if (iPlayerIx == PLAYER1) {
        destWIN.y = iCooYA;
        destLOS.y = iCooYB;
    } else {
        destWIN.y = iCooYB;
        destLOS.y = iCooYA;
    }

    uint64_t iInitialTick = SDL_GetTicks();
    uint64_t iFlashTickStart = iInitialTick;
    do {
        for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
            SDL_PumpEvents();
            if (SDL_GetMouseState(NULL, NULL)) {
                return;  // stop the animation
            }

            // this is the only value to be changed if leds are moved
            destWIN.x = 400 + 16 * iManoNum;
            destLOS.x = destWIN.x;

            if (bFlash) {
                // winner is  ON
                destWIN.w = _p_AnImages[IMG_LEDGREEN_ON]->w;
                destWIN.h = _p_AnImages[IMG_LEDGREEN_ON]->h;
                SDL_BlitSurface(_p_AnImages[IMG_LEDGREEN_ON], NULL, _p_Screen,
                                &destWIN);

            } else {
                // winner  off for flashing
                destWIN.w = _p_AnImages[IMG_LEDGREEN_OFF]->w;
                destWIN.h = _p_AnImages[IMG_LEDGREEN_OFF]->h;
                SDL_BlitSurface(_p_AnImages[IMG_LEDGREEN_OFF], NULL, _p_Screen,
                                &destWIN);
            }

            if (!bIsPata) {
                // loser is red, not flash
                destLOS.w = _p_AnImages[IMG_LED_REDON]->w;
                destLOS.h = _p_AnImages[IMG_LED_REDON]->h;
                SDL_BlitSurface(_p_AnImages[IMG_LED_REDON], NULL, _p_Screen,
                                &destLOS);
            } else {
                // giocata patada
                if (bFlash) {
                    // players are  ON
                    destLOS.w = _p_AnImages[IMG_LED_BLUEON]->w;
                    destLOS.h = _p_AnImages[IMG_LED_BLUEON]->h;
                    SDL_BlitSurface(_p_AnImages[IMG_LED_BLUEON], NULL,
                                    _p_Screen, &destLOS);

                    destWIN.w = _p_AnImages[IMG_LED_BLUEON]->w;
                    destWIN.h = _p_AnImages[IMG_LED_BLUEON]->h;
                    SDL_BlitSurface(_p_AnImages[IMG_LED_BLUEON], NULL,
                                    _p_Screen, &destWIN);

                } else {
                    // players also off
                    destLOS.w = _p_AnImages[IMG_LEDGREEN_OFF]->w;
                    destLOS.h = _p_AnImages[IMG_LEDGREEN_OFF]->h;
                    SDL_BlitSurface(_p_AnImages[IMG_LEDGREEN_OFF], NULL,
                                    _p_Screen, &destLOS);
                }
            }
        }
        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                          _p_Screen->pitch);  // sdl 2.0
        SDL_RenderClear(_p_sdlRenderer);
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);

        uint64_t iNowTick = SDL_GetTicks();
        iTickFlashDiff = (int)(iNowTick - iFlashTickStart);
        if (iTickFlashDiff > 90) {
            bFlash = !bFlash;
            iFlashTickStart = iNowTick;
        }
        iTickTot = (int)(iNowTick - iInitialTick);
    } while (iTickTot < 1000);
}

int InvidoGfx::animateCards() {
    SDL_srand(0);

    int rot;
    int xspeed;
    int yspeed;

    int GRAVITY = 1;
    int MAXY = _p_Screen->h;
    float BOUNCE = 0.8f;
    CardGfx cardGfx;

    do {
        rot = (int)SDL_rand(2);
        cardGfx.cardSpec.SetCardIndex((int)SDL_rand(40));

        cardGfx._x = (int)SDL_rand(_p_Screen->w);
        cardGfx._y = (int)SDL_rand(_p_Screen->h / 2);

        if (rot) {
            xspeed = -4;
        } else {
            xspeed = 4;
        }

        yspeed = 0;

        do  // while card is within the _p_Screen
        {
            SDL_PumpEvents();
            if (SDL_GetMouseState(NULL, NULL))
                return -1;  // stop the animation

            yspeed = yspeed + GRAVITY;
            cardGfx._x += xspeed;
            cardGfx._y += yspeed;

            if (cardGfx._y + _cardHeight > MAXY) {
                cardGfx._y = MAXY - _cardHeight;
                yspeed = (int)(-yspeed * BOUNCE);
            }

            cardGfx.DrawCard(_p_Screen);
            SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                              _p_Screen->pitch);  // sdl 2.0
            SDL_RenderClear(_p_sdlRenderer);
            SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
            SDL_RenderPresent(_p_sdlRenderer);
        } while ((cardGfx._x + 73 > 0) && (cardGfx._x < _p_Screen->w));
    } while (1);

    return 0;
}

void InvidoGfx::showOkMsgBox(LPCSTR strText) {
    // prepare the size of the box
    MesgBoxGfx MsgBox;
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w - 100;
    rctBox.h = 130;
    rctBox.y = (_p_Screen->h - rctBox.h) / 2;
    rctBox.x = (_p_Screen->w - rctBox.w) / 2;

    UpdateScreenCb fnUp = prepUpdateScreenCb();
    MsgBox.Initialize(&rctBox, _p_Screen, _p_FontStatus, MesgBoxGfx::TY_MBOK,
                      fnUp);
    MsgBox.Show(_p_Scene_background, "Ok", "", strText);
}

LPErrInApp InvidoGfx::loadCardPac() {
    Uint32 timetag;
    char describtion[100];
    Uint8 num_anims;
    Uint16 w, h;
    Uint16 frames;

    // int FRAMETICKS = (1000 / FPS);
    // int THINKINGS_PER_TICK = 1;

    std::string strFileName = lpszImageDir;
    strFileName += _p_DeckType->GetResFileName();

    SDL_IOStream* src = SDL_IOFromFile(strFileName.c_str(), "rb");
    if (src == 0) {
        char ErrBuff[512];
        sprintf(ErrBuff, "Error on load deck file %s", strFileName.c_str());
        return ERR_UTIL::ErrorCreate(ErrBuff);
    }
    SDL_ReadIO(src, describtion, 100);
    SDL_ReadU32LE(src, &timetag);
    SDL_ReadU8(src, &num_anims);
    // witdh of the picture (pac of 4 cards)
    SDL_ReadU16LE(src, &w);
    // height of the picture (pac of 10 rows of cards)
    SDL_ReadU16LE(src, &h);
    SDL_ReadU16LE(src, &frames);

    for (int i = 0; i < frames; i++) {
        Uint16 dummy;
        SDL_ReadU16LE(src, &dummy);
    }

    _p_Deck = IMG_Load_IO(src, true);
    if (!_p_Deck) {
        fprintf(stderr, "Cannot create deck: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetSurfaceColorKey(
        _p_Deck, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_Deck->format), NULL, 0, 128,
                   0));  // SDL 3.0

    _cardWidth = w / 4;
    _cardHeight = h / 10;

    return 0;
}

int InvidoGfx::showYesNoMsgBox(LPCSTR strText) {
    SDL_assert(_p_AlphaDisplay);

    // prepare the size of the box
    MesgBoxGfx MsgBox;
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w - 100;
    rctBox.h = 130;
    rctBox.y = (_p_Screen->h - rctBox.h) / 2;
    rctBox.x = (_p_Screen->w - rctBox.w) / 2;

    // show a mesage box with alpha
    UpdateScreenCb fnUp = prepUpdateScreenCb();
    MsgBox.Initialize(&rctBox, _p_Screen, _p_FontStatus,
                      MesgBoxGfx::TY_MB_YES_NO, fnUp);
    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    std::string strTextYes = _p_LangMgr->GetStringId(Languages::ID_YES);
    std::string strTextNo = _p_LangMgr->GetStringId(Languages::ID_NO);
    LPErrInApp err = MsgBox.Show(_p_AlphaDisplay, strTextYes.c_str(),
                                 strTextNo.c_str(), strText);

    return (int)MsgBox.GetResult();
}

void InvidoGfx::InitInvidoVsCPU() {
    if (_p_InvidoCore) {
        delete _p_InvidoCore;
        _p_InvidoCore = 0;
    }
    _p_InvidoCore = new InvidoCore();
    _p_InvidoCore->Create(NULL, 2);
    // g_pInvidoCore = _p_InvidoCore; // remove global g_pInvidoCore

    _p_InvidoCore->SetRandomSeed((unsigned)time(NULL));

    Player* pPlayer1 = _p_InvidoCore->GetPlayer(PLAYER1);
    Player* pPlayer2 = _p_InvidoCore->GetPlayer(PLAYER2);

    pPlayer1->SetType(PT_LOCAL);
    pPlayer1->SetName(GameSettings::GetSettings()->PlayerName.c_str());
    pPlayer1->SetLevel(HMI, this);

    pPlayer2->SetType(PT_MACHINE);
    pPlayer2->SetName("Re Adlinvidu");
    pPlayer2->SetLevel(ADVANCED, NULL);
    _p_MatchPoints = _p_InvidoCore->GetMatchPointsObj();

    TRACE_DEBUG("Partita tra %s e %s", pPlayer1->GetName(),
                pPlayer2->GetName());

    _isMatchTerminated = false;
}

void InvidoGfx::MatchLoop() {
    Mix_ChannelFinished(fnEffectTer);
    _p_InvidoCore->NewMatch();
    drawStaticScene();

    SDL_Event event;
    int done = 0;
    uint64_t uiLast_time;
    // uint32_t uiFrame = 0;
    uint64_t uiNowTime = 0;
    _DelayAction.Reset();

    uiLast_time = SDL_GetTicks();
    std::string strTextTmp;
    TRACE_DEBUG("Inizio partita loop \n");
    while (done == 0 && _isMatchTerminated == false) {
        // uiFrame++;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    // user want to exit the match
                    // show a messagebox for confirm
                    strTextTmp = _p_LangMgr->GetStringId(Languages::ASK_QUIT);
                    if (showYesNoMsgBox(strTextTmp.c_str()) ==
                        MesgBoxGfx::RES_YES) {
                        TRACE_DEBUG("Partita finita per scelta utente\n");
                        return;
                    }
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        done = 1;
                    }
                    handleKeyDownEvent(event);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    handleMouseDownEvent(event);
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    handleMouseMoveEvent(event);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    handleMouseUpEvent(event);
                    break;
            }
        }

        uiNowTime = SDL_GetTicks();
        if (uiNowTime > uiLast_time + (1000 / FPS)) {
            drawStaticScene();
            uiLast_time = uiNowTime;
        }

        // next action on the game
        if (_DelayAction.CanStart()) {
            _p_InvidoCore->NextAction();
        }

        // SDL 3.0
        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                          _p_Screen->pitch);
        SDL_RenderClear(_p_sdlRenderer);
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);
    }
}

void InvidoGfx::handleMouseDownEvent(SDL_Event& event) {
    int i;
    if (event.button.button == SDL_BUTTON_LEFT) {
        // check if the player have to play a card
        for (i = 0; i < NUM_CARDS_HAND; i++) {
            // check if a card was clicked
            if (_aPlayerCards[i].MouseInCard(event.button.x, event.button.y)) {
                // click on player card
                clickOnPlayerCard(i);
            }
        }
    } else if (event.button.button == SDL_BUTTON_RIGHT) {
        // mouse right
        // use the right button to show a popup menu
        if (_isPlayerCanPlay) {
            for (i = 0; i < NUM_CARDS_HAND; i++) {
                if (_aPlayerCards[i].State == CardGfx::CSW_ST_VISIBLE &&
                    _aPlayerCards[i].MouseInCard(event.button.x,
                                                 event.button.y)) {
                    // take care of Zorder
                    int iIndex_1 = i;
                    int iIndex_2 = i + 1;
                    int iZor_1 = _aPlayerCards[iIndex_1].m_iZOrder;
                    int iZor_2 = -1;
                    if (iIndex_2 < NUM_CARDS_HAND) {
                        if (_aPlayerCards[iIndex_2].MouseInCard(
                                event.button.x, event.button.y)) {
                            // overlap the second card
                            iZor_2 = _aPlayerCards[iIndex_2].m_iZOrder;
                        }
                    }
                    int iIndexCardSelected = iIndex_1;
                    if (iZor_2 > iZor_1) {
                        iIndexCardSelected = iIndex_2;
                    }

                    eSayPlayer eSay = SP_NOTHING;
                    showPopUpCallMenu(
                        _aPlayerCards[iIndexCardSelected].cardSpec,
                        event.button.x, event.button.y, &eSay);
                    if (eSay != SP_NOTHING) {
                        if (eSay == SP_VADODENTRO) {
                            vadoDentro(iIndexCardSelected);
                        } else {
                            INP_PlayerSay(eSay);
                            if (GameSettings::GetSettings()
                                    ->MusicEnabled) {  // Use MusicEnabled or
                                                       // something? Wait, check
                                                       // bMyCallEcho
                                // if
                                // (GameSettings::GetSettings()->All.bMyCallEcho)
                                // // g_Options.All.bMyCallEcho
                                int iMusId = _Map_id_EchoSay[eSay];
                                if (_p_MusicMgr->PlayEffect(iMusId)) {
                                    //_DelayAction.CheckPoint(
                                    //    2260, DelayNextAction::CHANGE_AVAIL);
                                }
                            }
                        }
                    }
                    // stop search other cards
                    break;
                }
            }
        }
    }
}

// showPopUpCallMenu
void InvidoGfx::showPopUpCallMenu(CardSpec& cardClicked, int iX, int iY,
                                  eSayPlayer* peSay) {
    TRACE_DEBUG("show popup menu\n");
    SDL_assert(peSay);
    SDL_assert(_p_AlphaDisplay);
    *peSay = SP_NOTHING;
    VCT_COMMANDS vct_cmd;
    SDL_assert(_p_InvidoCore);
    _p_InvidoCore->GetMoreCommands(vct_cmd, _playerGuiIndex);
    vct_cmd.push_back(SP_NOTHING);

    size_t iNumCmdsAval = vct_cmd.size();

    // prepare the size of the box
    PopUpMenuGfx PopUpMenu;
    SDL_Rect rctBox;
    rctBox.w = _p_Screen->w;  // max value, width is autocalculated
    rctBox.h = _p_Screen->h;  // max value hight is autocalculated
    rctBox.y = iY;
    rctBox.x = iX;

    // show a mesage box
    PopUpMenu.Show(&rctBox, _p_Screen, _p_FontText);
    SDL_BlitSurface(_p_Screen, NULL, _p_AlphaDisplay, NULL);

    for (size_t i = 0; i < iNumCmdsAval; i++) {
        eSayPlayer eSay = vct_cmd[i];
        std::string strTmp = _Map_bt_Say[eSay];

        PopUpMenu.AddLineText(strTmp.c_str());
    }

    // Modal loop for the popup menu
    bool bDone = false;
    while (!bDone) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            float mx, my;
            SDL_GetMouseState(&mx, &my);
            SDL_Point mousePos;
            mousePos.x = (int)mx;
            mousePos.y = (int)my;
            PopUpMenu.HandleEvent(&event, mousePos);
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                (event.type == SDL_EVENT_KEY_DOWN &&
                 (event.key.key == SDLK_RETURN ||
                  event.key.key == SDLK_ESCAPE))) {
                bDone = true;
            }
        }
        SDL_BlitSurface(_p_AlphaDisplay, NULL, _p_Screen, NULL);
        PopUpMenu.DrawCtrl(_p_Screen);

        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                          _p_Screen->pitch);
        SDL_RenderClear(_p_sdlRenderer);
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);
        SDL_Delay(10);
    }

    // menu is terminated
    if (PopUpMenu.MenuIsSelected()) {
        // the user choice a menu
        int iIndexSel = PopUpMenu.GetSlectedIndex();
        if (iIndexSel >= 0 && (size_t)iIndexSel < vct_cmd.size()) {
            *peSay = vct_cmd[iIndexSel];
        }
    } else {
        *peSay = SP_NOTHING;
    }

    TRACE_DEBUG("END popup menu\n");
    drawStaticScene();
}

void InvidoGfx::clickOnPlayerCard(int iIndex) {
    TRACE_DEBUG("card clicked %d\n", iIndex);
    if (_isPlayerCanPlay &&
        (_aPlayerCards[iIndex].State == CardGfx::CSW_ST_VISIBLE)) {
        _p_InvidoCore->Player_playCard(
            PLAYER_ME, _aPlayerCards[iIndex].cardSpec.GetCardInfo());
        _isPlayerCanPlay = false;
    }
}

void InvidoGfx::vadoDentro(int cardIx) {
    TRACE_DEBUG("Card vado dentro %d\n", cardIx);
    if (_isPlayerCanPlay &&
        (_aPlayerCards[cardIx].State == CardGfx::CSW_ST_VISIBLE)) {
        _cardVadoDentroIndex = cardIx;
        _p_InvidoCore->Player_vaDentro(
            PLAYER_ME, _aPlayerCards[cardIx].cardSpec.GetCardInfo());
        _isPlayerCanPlay = false;
    }
}

void InvidoGfx::renderScreen() {
    SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_Screen->pixels,
                      _p_Screen->pitch);  // sdl 2.0
    SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
    SDL_RenderPresent(_p_sdlRenderer);
}

void InvidoGfx::drawVadoDentroCard(CardGfx* pCard) {
    drawStaticScene();
    pCard->SetSymbolTocard(CardGfx::CSW_ST_SYMBOL);
    renderCard(pCard);
    renderScreen();
}

void InvidoGfx::drawPlayedCard(CardGfx* pCard) {
    int iIndexToUse = 0;
    if (_CardsTable[0].State == CardGfx::CSW_ST_INVISIBLE) {
        // first card  played
        iIndexToUse = 0;
    } else {
        // second card played
        iIndexToUse = 1;
    }
    drawStaticScene();

    _CardsTable[iIndexToUse].Copy(pCard);
    renderCard(&_CardsTable[iIndexToUse]);
    renderCard(pCard);
    renderScreen();
}

////////////////////////////////////////
//       HandleMouseMoveEvent
void InvidoGfx::handleMouseMoveEvent(SDL_Event& event) {
    SDL_Point mousePos;
    mousePos.x = (int)event.motion.x;
    mousePos.y = (int)event.motion.y;
    for (int i = 0; i < NUMOFBUTTON; i++) {
        _p_btArrayCmd[i]->MouseMove(&event, mousePos);
    }
}

////////////////////////////////////////
//       HandleMouseUpEvent
void InvidoGfx::handleMouseUpEvent(SDL_Event& event) {
    SDL_Point mousePos;
    mousePos.x = (int)event.button.x;
    mousePos.y = (int)event.button.y;
    for (int i = 0; i < NUMOFBUTTON; i++) {
        _p_btArrayCmd[i]->MouseUp(&event, mousePos);
    }
}

void InvidoGfx::handleKeyDownEvent(SDL_Event& event) {
    if (event.key.key == SDLK_n) {
        // startNewMatch();
        drawStaticScene();
    }
    if (event.key.key == SDLK_a) {
        animateCards();
    };  // Test animation
    if (event.key.key == SDLK_r) {
        // refresh
        drawStaticScene();
    };  // Refresh
}

void InvidoGfx::showPlayerMarkup(int iPlayerIx) {
    SDL_Rect dest;
    if (iPlayerIx == PLAYER1) {
        dest.x = 350;
        dest.y = 500;
    } else if (iPlayerIx == PLAYER2) {
        dest.x = 350;
        dest.y = 50;
    } else {
        SDL_assert(0);
    }
    dest.w = _p_AnImages[IMG_TOCCA_PLAYER]->w;
    dest.h = _p_AnImages[IMG_TOCCA_PLAYER]->h;
    SDL_BlitSurface(_p_AnImages[IMG_TOCCA_PLAYER], NULL, _p_Screen, &dest);
}

void InvidoGfx::showPointsPlayer(int iPlayerIx, VCT_INT& vct_Points) {
    int iCurr_Y;
    SDL_Rect dest;
    int iInitial_X1 = 550;
    int iInitial_X2 = iInitial_X1 + 120;
    if (iPlayerIx == PLAYER1) {
        dest.x = iInitial_X1;
    } else {
        dest.x = iInitial_X2;
    }
    int iInitial_X = dest.x;
    iCurr_Y = 50;

    int iNumPoints = (int)vct_Points.size();
    int iCaneliOnLine = 0;
    int iEggOnLine = 0;
    int iY_NextCanela = iCurr_Y;
    int iY_NextEgg = iCurr_Y;
    int iCanelaInterl = 12;
    int iX_NextEgg = iInitial_X;
    for (int i = 0; i < iNumPoints; i++) {
        int iCurrPoint = vct_Points[i];
        if (iCurrPoint == 1) {
            dest.x = iInitial_X;
            iCaneliOnLine++;
            // draw canela
            if (iCaneliOnLine == 1) {
                if (iY_NextCanela <= iY_NextEgg) {
                    if (iEggOnLine == 0) {
                        iY_NextCanela = iY_NextEgg;
                    } else {
                        iY_NextCanela =
                            iY_NextEgg + _p_AnImages[IMG_UOVO]->h + 3;
                    }
                }

                dest.y = iY_NextCanela;
                dest.w = _p_AnImages[IMG_CANELA]->w;
                dest.h = _p_AnImages[IMG_CANELA]->h;
                SDL_BlitSurface(_p_AnImages[IMG_CANELA], NULL, _p_Screen,
                                &dest);

                iY_NextCanela += iCanelaInterl;
            } else if (iCaneliOnLine == 2) {
                dest.y = iY_NextCanela;
                dest.w = _p_AnImages[IMG_CANELA]->w;
                dest.h = _p_AnImages[IMG_CANELA]->h;
                SDL_BlitSurface(_p_AnImages[IMG_CANELA], NULL, _p_Screen,
                                &dest);

                iY_NextCanela -= iCanelaInterl;
            } else if (iCaneliOnLine == 3) {
                dest.y = iY_NextCanela;
                dest.w = _p_AnImages[IMG_CANELA_OBL]->w;
                dest.h = _p_AnImages[IMG_CANELA_OBL]->h;
                SDL_BlitSurface(_p_AnImages[IMG_CANELA_OBL], NULL, _p_Screen,
                                &dest);
                if (iEggOnLine != 1) {
                    iY_NextCanela += dest.h + 3;
                } else {
                    iY_NextCanela = iY_NextEgg + _p_AnImages[IMG_UOVO]->h + 3;
                }
                iCaneliOnLine = 0;
            } else {
                SDL_assert(0);
            }
        } else if (iCurrPoint == 3) {
            // draw egg
            iEggOnLine++;
            if (iEggOnLine <= 1) {
                // first egg on the line
                if (iCaneliOnLine == 0) {
                    if (iY_NextEgg < iY_NextCanela) {
                        iY_NextEgg = iY_NextCanela;
                    }
                } else if (iCaneliOnLine == 1) {
                    int iPossY =
                        iY_NextCanela + _p_AnImages[IMG_CANELA_OBL]->h + 3;
                    if (iY_NextEgg < iPossY) {
                        iY_NextEgg = iPossY;
                    }
                } else {
                    // 2 caneli drawn
                    int iPossY =
                        iY_NextCanela + _p_AnImages[IMG_CANELA_OBL]->h + 3;
                    if (iY_NextEgg < iPossY) {
                        iY_NextEgg = iPossY;
                    }
                }
                dest.y = iY_NextEgg;
                dest.x = iInitial_X;
                iX_NextEgg += 40;

            } else if (iEggOnLine <= 2) {
                // second egg
                iEggOnLine = 0;
                dest.y = iY_NextEgg;
                dest.x = iX_NextEgg;
                iY_NextEgg += _p_AnImages[IMG_UOVO]->h + 3;
                iX_NextEgg = iInitial_X;
            }
            dest.w = _p_AnImages[IMG_UOVO]->w;
            dest.h = _p_AnImages[IMG_UOVO]->h;
            SDL_BlitSurface(_p_AnImages[IMG_UOVO], NULL, _p_Screen, &dest);
        } else {
            SDL_assert(0);
        }
    }
}

void InvidoGfx::showManoScore(bool bIsPlayed, int iPlayerIx, bool bIsPata,
                              int iManoNum) {
    SDL_Rect dest;
    SDL_Rect destOff;
    dest.x = 400 + 16 * iManoNum;
    destOff.x = dest.x;
    int iCooYA = _p_Screen->h - 30;
    int iCooYB = 20;

    if (iPlayerIx == PLAYER1 || !bIsPlayed || bIsPata) {
        dest.y = iCooYA;
        destOff.y = iCooYB;
    } else if (iPlayerIx == PLAYER2) {
        dest.y = iCooYB;
        destOff.y = iCooYA;
    } else {
        SDL_assert(0);
    }
    if (bIsPlayed) {
        // winner is ON
        dest.w = _p_AnImages[IMG_LEDGREEN_ON]->w;
        dest.h = _p_AnImages[IMG_LEDGREEN_ON]->h;
        SDL_BlitSurface(_p_AnImages[IMG_LEDGREEN_ON], NULL, _p_Screen, &dest);
        if (bIsPata) {
            // patada, both blue
            destOff.w = _p_AnImages[IMG_LED_BLUEON]->w;
            destOff.h = _p_AnImages[IMG_LED_BLUEON]->h;
            SDL_BlitSurface(_p_AnImages[IMG_LED_BLUEON], NULL, _p_Screen,
                            &destOff);

            dest.w = _p_AnImages[IMG_LED_BLUEON]->w;
            dest.h = _p_AnImages[IMG_LED_BLUEON]->h;
            SDL_BlitSurface(_p_AnImages[IMG_LED_BLUEON], NULL, _p_Screen,
                            &dest);
        } else {
            // loser is red
            destOff.w = _p_AnImages[IMG_LED_REDON]->w;
            destOff.h = _p_AnImages[IMG_LED_REDON]->h;
            SDL_BlitSurface(_p_AnImages[IMG_LED_REDON], NULL, _p_Screen,
                            &destOff);
        }
    } else {
        // mano was not played (both off)
        dest.w = _p_AnImages[IMG_LEDGREEN_OFF]->w;
        dest.h = _p_AnImages[IMG_LEDGREEN_OFF]->h;
        SDL_BlitSurface(_p_AnImages[IMG_LEDGREEN_OFF], NULL, _p_Screen, &dest);
        destOff.w = _p_AnImages[IMG_LEDGREEN_OFF]->w;
        destOff.h = _p_AnImages[IMG_LEDGREEN_OFF]->h;
        SDL_BlitSurface(_p_AnImages[IMG_LEDGREEN_OFF], NULL, _p_Screen,
                        &destOff);
    }
}

void InvidoGfx::guiPlayerTurn(int iPlayer) {
    SDL_assert(_p_MatchPoints);
    _isPlayerCanPlay = true;

    _playerThatHaveMarkup = iPlayer;

    // update the screen
    drawStaticScene();

    Player* pPlayer = _p_InvidoCore->GetPlayer(iPlayer);

    if (GameSettings::GetSettings()
            ->MusicEnabled) {  // Verify if this is the right field for
                               // verbosity/logging
        TRACE_DEBUG("Player che deve giocare %d: %s\n", iPlayer,
                    pPlayer->GetName());
    }
}

void InvidoGfx::showCurrentScore() {
    // mano score
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        bool bIsPata;
        bool bIsPlayed;
        int iPlayerIx;
        _p_MatchPoints->GetManoInfo(iManoNum, &iPlayerIx, &bIsPlayed, &bIsPata);
        showManoScore(bIsPlayed, iPlayerIx, bIsPata, iManoNum);
    }

    // grid
    int iX1 = 540;
    int iY_oriz = 45;
    int iX_end = iX1 + 240;
    int iX_vertical = iX1 + (iX_end - iX1) / 2;
    int iY1 = iY_oriz - 30;
    int iY_end = iY1 + 300;

    int iBackOff = 10;

    // score background
    GFX_UTIL::DrawStaticSpriteEx(
        _p_Screen, 0, 0, iX_end - iX1 + iBackOff, iY_end - iY1 + iBackOff + 30,
        iX1 - iBackOff / 2, iY1 - iBackOff / 2, _p_Surf_Bar);

    SDL_Rect dest;

    // vertical line
    int i;
    for (i = iY1; i < iY_end; i += _p_AnImages[IMG_VERTICAL]->h) {
        dest.x = iX_vertical - 2;
        dest.y = i;
        dest.w = _p_AnImages[IMG_VERTICAL]->w;
        dest.h = _p_AnImages[IMG_VERTICAL]->h;

        SDL_BlitSurface(_p_AnImages[IMG_VERTICAL], NULL, _p_Screen, &dest);
    }
    // horizontal
    dest.w = _p_AnImages[IMG_HORIZONTAL]->w;
    dest.h = _p_AnImages[IMG_HORIZONTAL]->h;
    for (i = iX1; i < iX_end; i += _p_AnImages[IMG_HORIZONTAL]->w) {
        dest.x = i;
        dest.y = iY_oriz;

        SDL_BlitSurface(_p_AnImages[IMG_HORIZONTAL], NULL, _p_Screen, &dest);
    }

    // name on grid - player 1
    Player* pPlayer = _p_InvidoCore->GetPlayer(PLAYER1);
    STRING strTmp = pPlayer->GetName();
    int iLenName = (int)strTmp.length();
    GFX_UTIL::DrawString(_p_Screen, pPlayer->GetName(),
                         iX_vertical - (9 * iLenName), iY1,
                         GFX_UTIL_COLOR::White, _p_FontText);
    // player 2
    pPlayer = _p_InvidoCore->GetPlayer(PLAYER2);
    GFX_UTIL::DrawString(_p_Screen, pPlayer->GetName(), iX_vertical + 10, iY1,
                         GFX_UTIL_COLOR::White, _p_FontText);

    // current giocata score
    eGiocataScoreState eCurrScore = _p_MatchPoints->GetCurrScore();
    STRING lpsNamePoints = _MapPunti[eCurrScore];
    if (_p_MatchPoints->IsGiocataMonte()) {
        lpsNamePoints = _p_LangMgr->GetStringId(Languages::ID_S_AMONTE).c_str();
    }
    char buffTmp[256];
    sprintf(buffTmp, "%s: %s",
            _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
            lpsNamePoints.c_str());
    int tx, ty;
    TTF_GetStringSize(_p_FontText, buffTmp, 0, &tx, &ty);
    int iX_posCurrScore = iX_vertical - tx / 2;
    int iY_posCurrScore = iY_end + 10;
    GFX_UTIL::DrawString(_p_Screen, buffTmp, iX_posCurrScore, iY_posCurrScore,
                         GFX_UTIL_COLOR::White, _p_FontText);

    // player score
    int iNumGiocate = _p_MatchPoints->GetNumGiocateInCurrMatch();
    VCT_INT vct_Point_pl1;
    for (int j = 0; j < NUM_PLAY_INVIDO_2; j++) {
        vct_Point_pl1.clear();
        for (int iNumGio = 0; iNumGio < iNumGiocate; iNumGio++) {
            GiocataInfo GioInfo;
            _p_MatchPoints->GetGiocataInfo(iNumGio, &GioInfo);

            if (GioInfo.score > 0) {
                if (GioInfo.playerIndex == j) {
                    if (GioInfo.score == SC_TRASMAS) {
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                    } else if (GioInfo.score == SC_TRASMASNOEF) {
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                    } else if (GioInfo.score == SC_FUERAJEUQ) {
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                    } else if (GioInfo.score == SC_PARTIDA) {
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                        vct_Point_pl1.push_back(3);
                    } else {
                        vct_Point_pl1.push_back(GioInfo.score);
                    }
                }
            }
        }
        showPointsPlayer(j, vct_Point_pl1);
    }
}

void InvidoGfx::enableCmds() {
    VCT_COMMANDS vct_cmd;
    SDL_assert(_p_InvidoCore);
    _p_InvidoCore->GetAdmittedCommands(vct_cmd, _playerGuiIndex);

    // reset button to default strings
    size_t iNumCmd = vct_cmd.size();
    enableOnlyCmdButtons(iNumCmd);

    for (size_t i = 0; i < iNumCmd; i++) {
        // set the command with the new command
        eSayPlayer eSay = vct_cmd[i];
        STRING strTmp = _Map_bt_Say[eSay];
        setCmdButton(i, eSay, strTmp.c_str());
    }
}

void InvidoGfx::enableOnlyCmdButtons(size_t iNumButt) {
    int i, j;
    for (i = 0; i < (int)iNumButt; i++) {
        // enable buttons with commands
        _p_btArrayCmd[i]->Enable(true);
        _p_btArrayCmd[i]->SetVisibleState(ButtonGfx::VISIBLE);
    }
    for (j = i; j < NUMOFBUTTON; j++) {
        // the rest of buttons are disabled
        _p_btArrayCmd[j]->Enable(false);
        _p_btArrayCmd[j]->SetVisibleState(ButtonGfx::INVISIBLE);
        _p_btArrayCmd[j]->SetButtonText("-");
        _p_btArrayCmd[j]->DrawButton(_p_Screen);
    }
}

void InvidoGfx::setCmdButton(size_t iButtonIndex, eSayPlayer eSay,
                             LPCSTR strCaption) {
    if (iButtonIndex >= 0 && iButtonIndex < NUMOFBUTTON) {
        _p_btArrayCmd[iButtonIndex]->SetButtonText(strCaption);
        _CmdDet[iButtonIndex] = eSay;
        _p_btArrayCmd[iButtonIndex]->DrawButton(_p_Screen);
    } else {
        SDL_assert(0);
    }
}

void InvidoGfx::ButCmdClicked(int iButID) {
    if (iButID >= 0 && iButID < NUMOFBUTTON) {
        // first 6 ids are say commands buttons
        eSayPlayer eSay = _CmdDet[iButID];
        if (_p_InvidoCore->Player_saySomething(_playerGuiIndex, eSay)) {
            // said something admitted
            // disable all buttons
            enableOnlyCmdButtons(0);
            drawStaticScene();
            // play what i say (echo mode)
            if (GameSettings::GetSettings()->MusicEnabled) {
                int iMusId = _Map_id_EchoSay[eSay];
                if (_p_MusicMgr->PlayEffect(iMusId)) {
                    _DelayAction.CheckPoint(2260,
                                            DelayNextAction::CHANGE_AVAIL);
                }
            }
        }
    }
}

void InvidoGfx::NtfyTermEff(int iCh) { _DelayAction.ChangeCurrDelay(50); }

// ***************************************************
//********  interface invido core  callback **********
// ***************************************************

void InvidoGfx::ALG_Play() {
    guiPlayerTurn(_playerGuiIndex);
    enableCmds();
}

void InvidoGfx::ALG_HaveToRespond() {
    TRACE_DEBUG("%s\n", _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str());
    enableCmds();
}

void InvidoGfx::ALG_PlayerHasSaid(int iPlayerIx, eSayPlayer SaySomeThing) {
    _p_balGfx->StartShow("", 0);

    if (iPlayerIx == _opponentIndex) {
        // viene solo ripetuta la voce dell'avversario. Quella del giocatore è
        // in echo
        Player* pPlayer = _p_InvidoCore->GetPlayer(iPlayerIx);
        STRING lpsNameSay = _Map_fb_Say[SaySomeThing];
        TRACE_DEBUG("Player: %s, Said: %s\n", pPlayer->GetName(),
                    lpsNameSay.c_str());

        _p_balGfx->StartShow(lpsNameSay.c_str());
        if (GameSettings::GetSettings()->MusicEnabled) {
            // say also with music
            int iMusId = _Map_idSynth_Say[SaySomeThing];
            _p_MusicMgr->PlayEffect(iMusId);
            //_DelayAction.CheckPoint(60, DelayNextAction::CHANGE_AVAIL);
        }
    } else {
        enableCmds();
    }
}

void InvidoGfx::ALG_PlayerHasVadoDentro(int iPlayerIx) {
    if (iPlayerIx == _playerGuiIndex && _cardVadoDentroIndex >= 0 &&
        _cardVadoDentroIndex < NUM_CARDS_HAND) {
        if (_aPlayerCards[_cardVadoDentroIndex].State ==
            CardGfx::CSW_ST_VISIBLE) {
            TRACE_DEBUG("card played %s\n",
                        _aPlayerCards[_cardVadoDentroIndex].cardSpec.GetName());

            drawVadoDentroCard(&_aPlayerCards[_cardVadoDentroIndex]);
        }
        //_DelayAction.CheckPoint(600, DelayNextAction::NOCHANGE);
    } else if (iPlayerIx == _opponentIndex) {
        std::string lpsNameSay = _Map_fb_Say[SP_VADODENTRO];
        _p_balGfx->StartShow(lpsNameSay.c_str());
        CardSpec Card;
        Card.SetCardIndex(3);
        opponentHasPlayedCard(Card, true);
    } else {
        SDL_assert(0);
    }
}

void InvidoGfx::opponentHasPlayedCard(CardSpec& Card, bool vadoDentro) {
    bool bFound = false;
    for (int iIndex = 0; !bFound && iIndex < NUM_CARDS_HAND; iIndex++) {
        if (_aOpponentCards[iIndex].State == CardGfx::CSW_ST_BACK) {
            if (vadoDentro) {
                TRACE_DEBUG("Opponent va dentro, draw it\n");
                drawVadoDentroCard(&_aOpponentCards[iIndex]);
            } else {
                TRACE_DEBUG("card played %s\n", Card.GetName());
                _aOpponentCards[iIndex].State = CardGfx::CSW_ST_VISIBLE;
                _aOpponentCards[iIndex].cardSpec = Card;
                drawPlayedCard(&_aOpponentCards[iIndex]);
            }
            bFound = true;
        }
    }
    SDL_assert(bFound);
    Player* pPlayer = _p_InvidoCore->GetPlayer(_opponentIndex);
    TRACE_DEBUG("%s %s ha giocato %s\n", lpszCST_INFO, pPlayer->GetName(),
                Card.GetName());
    int iNumCardPlayed = _p_MatchPoints->GetCurrNumCardPlayed();
    if (iNumCardPlayed == 1) {
        // first card played from opponent, don't need a delay
        //_DelayAction.CheckPoint(50, DelayNextAction::NOCHANGE);

    } else {
        // opponent was not the first, delay action to show a little the current
        // table
        //_DelayAction.CheckPoint(600, DelayNextAction::NOCHANGE);
    }
}

void InvidoGfx::ALG_PlayerHasPlayed(int iPlayerIx, const CARDINFO* pCard) {
    // disable ballon
    _p_balGfx->StartShow("", 0);

    CardSpec Card;
    Card.SetCardInfo(*pCard);

    // markup player that have to play
    Player* pPlayer = 0;

    _p_InvidoCore->GetPlayerInPlaying(&pPlayer);
    SDL_assert(pPlayer);

    if (pPlayer) {
        _playerThatHaveMarkup = pPlayer->GetIndex();
    }

    bool bFound = false;
    if (iPlayerIx == _opponentIndex) {
        // opponent play a card
        opponentHasPlayedCard(Card, false);

    } else if (iPlayerIx == _playerGuiIndex) {
        // HMI has played correctly
        for (int iIndex = 0; !bFound && iIndex < NUM_CARDS_HAND; iIndex++) {
            if (_aPlayerCards[iIndex].cardSpec == Card) {
                TRACE_DEBUG("card played %s\n", Card.GetName());

                drawPlayedCard(&_aPlayerCards[iIndex]);
                bFound = true;
            }
        }
        // min dealy before cpu play
        //_DelayAction.CheckPoint(600, DelayNextAction::NOCHANGE);

        SDL_assert(bFound);
    } else {
        SDL_assert(0);
    }
    if (bFound) {
        // card was played correctly
        // make a feedback
        _p_MusicMgr->PlayEffect(MusicManager::SND_EFC_CLICK);
    }
}

void InvidoGfx::ALG_NewGiocata(const CARDINFO* pCardArray, int iNumOfCards,
                               int iPlayerIx) {
    TRACE_DEBUG("ALG_NewGiocata\n");
    _isPlayerCanPlay = false;

    SDL_assert(iNumOfCards == NUM_CARDS_HAND);

    // NOTE: to set cards is better to use pCardArray.
    // Cards of the opponent are not yet set, so we can't display it.
    // This is correct because the Gfx engine operate like a player and not have
    // to know the opponent cards

    // player
    CardSpec tmpCardSpec;
    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        tmpCardSpec.SetCardInfo(pCardArray[i]);
        _aPlayerCards[i].cardSpec = tmpCardSpec;  // overloaded operator = that
                                                  // copy all important fields
        _aPlayerCards[i].State = CardGfx::CSW_ST_VISIBLE;
    }

    // opponent
    for (int j = 0; j < NUM_CARDS_HAND; j++) {
        _aOpponentCards[j].State = CardGfx::CSW_ST_BACK;
    }

    // cards played
    for (int k = 0; k < NUM_CARDS_PLAYED; k++) {
        _CardsTable[k].State = CardGfx::CSW_ST_INVISIBLE;
    }

    animateBeginGiocata();
}

void InvidoGfx::ALG_ManoEnd(I_MatchScore* pScore) {
    SDL_assert(pScore);
    _isPlayerCanPlay = false;

    int iPlayerIx = pScore->GetManoWinner();

    // necessary to show the cards played on the table
    SDL_Delay(500);

    // update the screen
    drawStaticScene();
    if (pScore->IsManoPatada()) {
        PlayersOnTable* pTable = _p_InvidoCore->GetTable();

        _playerThatHaveMarkup = pTable->GetFirstOnTrick();
        Player* pPlayer = pTable->GetPlayerIndex(_playerThatHaveMarkup);

        // Mano patada, tocca a
        TRACE_DEBUG("%s %s %s\n", lpszCST_INFO,
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                    pPlayer->GetName());

        // animation of pata, use an index outside the player table
        animateManoEnd(21);
    } else {
        Player* pPlayer = _p_InvidoCore->GetPlayer(iPlayerIx);
        // Mano vinta da
        TRACE_DEBUG("%s %s %s\n", lpszCST_INFO,
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                    pPlayer->GetName());

        animateManoEnd(iPlayerIx);
        _playerThatHaveMarkup = iPlayerIx;
    }

    // cards played
    for (int k = 0; k < NUM_CARDS_PLAYED; k++) {
        _CardsTable[k].State = CardGfx::CSW_ST_INVISIBLE;
    }

    drawStaticScene();
}

void InvidoGfx::ALG_GiocataEnd(I_MatchScore* pScore) {
    int iPlayerIx = pScore->GetGiocataWinner();

    bool bIsPata = pScore->IsGiocataPatada();
    STRING strMsgFinGiocata;
    if (bIsPata) {
        // giocata patada
        TRACE_DEBUG("%s %s\n", lpszCST_INFO,
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str());
        strMsgFinGiocata = _p_LangMgr->GetStringId(Languages::ID_SCORE);
    } else if (pScore->IsGiocataMonte()) {
        // giocata a monte
        TRACE_DEBUG("%s %s\n", lpszCST_INFO,
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str());
        strMsgFinGiocata = _p_LangMgr->GetStringId(Languages::ID_SCORE);
        bIsPata = true;
    } else {
        // giocata with a winner
        char buffText[512];

        int iPlayLoser;
        if (iPlayerIx == _playerGuiIndex) {
            iPlayLoser = _opponentIndex;
        } else {
            iPlayLoser = _playerGuiIndex;
        }
        Player* pPlayer = _p_InvidoCore->GetPlayer(iPlayerIx);
        Player* pPlLoser = _p_InvidoCore->GetPlayer(iPlayLoser);

        // Giocata vinta da
        TRACE_DEBUG("%s %s %s (%s %d)\n", lpszCST_INFO,
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                    pPlayer->GetName(),
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                    pScore->GetCurrScore());
        sprintf(buffText, "%s \"%s\" (%s %d)",
                _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                pPlayer->GetName(),
                _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                pScore->GetCurrScore());
        // punti
        TRACE_DEBUG("%s %s %s %d, %s %s %d\n", lpszCST_SCORE,
                    pPlayer->GetName(),
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                    pScore->GetPointsPlayer(iPlayerIx), pPlLoser->GetName(),
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                    pScore->GetPointsPlayer(iPlayLoser));

        strMsgFinGiocata = buffText;
    }
    _isPlayerCanPlay = false;
    drawStaticScene();
    animGiocataEnd(iPlayerIx, bIsPata);
    showOkMsgBox(strMsgFinGiocata.c_str());
}

void InvidoGfx::ALG_MatchEnd(I_MatchScore* pScore) {
    int iPlayerIx = pScore->GetMatchWinner();
    int iPlayLoser;
    if (iPlayerIx == _playerGuiIndex) {
        iPlayLoser = _opponentIndex;
    } else {
        iPlayLoser = _playerGuiIndex;
    }
    Player* pPlayer = _p_InvidoCore->GetPlayer(iPlayerIx);
    _isPlayerCanPlay = false;

    // partita finita. player vince x:x
    char buff[256];
    sprintf(buff, "%s. %s %s  %d : %d.",
            _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
            pPlayer->GetName(),
            _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
            pScore->GetPointsPlayer(iPlayerIx),
            pScore->GetPointsPlayer(iPlayLoser));

    TRACE_DEBUG("%s %s\n", lpszCST_INFO, buff);

    drawStaticScene();

    showOkMsgBox(buff);

    _isMatchTerminated = true;
}

void InvidoGfx::ALG_GicataScoreChange(eGiocataScoreState eNewScore) {
    STRING lpsNamePoints = _MapPunti[eNewScore];
    // Punteggio della giocata ora è:
    TRACE_DEBUG("%s %s: %s\n", lpszCST_INFO,
                _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str(),
                lpsNamePoints.c_str());
}

void InvidoGfx::ALG_PlayerSaidWrong(int iPlayerIx) {
    if (iPlayerIx == _playerGuiIndex) {
        // Quello che hai chiamato non è corretto
        TRACE_DEBUG("%s, %s\n", lpszCST_SU,
                    _p_LangMgr->GetStringId(Languages::ID_SCORE).c_str());
    }
}

void InvidoGfx::INP_PlayerSay(eSayPlayer eSay) {
    _p_InvidoCore->Player_saySomething(_playerGuiIndex, eSay);
}
