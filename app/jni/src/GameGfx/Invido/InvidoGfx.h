#ifndef __CINVIDO_GFX_H
#define __CINVIDO_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <map>

#include "AlgPlayerInterface.h"
#include "CardGfx.h"
#include "InvidoCore.h"
#include "Traits.h"
#include "Config.h"
#include "ErrorInfo.h"

using namespace traits;

enum {
    CRD_MAZZOALTRECARTE = 0,
    CRD_PLAYER1,
    CRD_PLAYER2,
    CRD_TAVOLO_1,
    CRD_TAVOLO_2,
    CRD_MAZZO_PLAYER1,
    CRD_MAZZO_PLAYER2,
};

class InvidoCore;
class DeckType;
class ButtonGfx;
class BalloonGfx;
class Languages;
class MusicManager;

typedef std::map<eGiocataScoreState, STRING> MAP_SCORE;
typedef std::map<eSayPlayer, STRING> MAP_SAY;
typedef std::map<eSayPlayer, int> MAP_INTID;
typedef std::vector<int> VCT_INT;

class InvidoGfx : public I_ALG_Player {
   private:
    enum {
        IMG_TOCCA_PLAYER,
        IMG_LEDGREEN_OFF,
        IMG_LEDGREEN_ON,
        IMG_CANELA,
        IMG_UOVO,
        IMG_VERTICAL,
        IMG_CORNER_DL,
        IMG_CORNER_DR,
        IMG_CORNER_UL,
        IMG_CORNER_UR,
        IMG_HORIZONTAL,
        IMG_LED_BLUEON,
        IMG_LED_REDON,
        IMG_CANELA_OBL,
        IMG_BALLOON,
        IMG_BALL_ARROW_UP,
        NUM_ANIMAGES
    };
    enum { NUMOFBUTTON = 6 };

   public:
    InvidoGfx();
    ~InvidoGfx();
    LPErrInApp Initialize(SDL_Surface* pScreen, UpdateScreenCb& fnUpdateScreen,
                          SDL_Window* pWindow, SDL_Surface* pSceneBackground,
                          UpdateHighScoreCb& fnUpdateHighScore);
    LPErrInApp OnResize(SDL_Surface* pScreen);
    LPErrInApp HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos);
    LPErrInApp HandleIterate(bool& done);
    LPErrInApp Show();

    void MatchLoop();
    void InitInvidoVsCPU();
    void INP_PlayerSay(eSayPlayer eSay);
    void ButCmdClicked(int iButID);
    void NtfyTermEff(int iCh);
    Player* GetPlayer(int iIndex) { return _p_InvidoCore->GetPlayer(iIndex); }

    // Interface I_ALG_Player (callback functions from core invido engine)
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore) {}
    virtual void ALG_SetPlayerIndex(int iIndex) { _playerGuiIndex = iIndex; }
    virtual void ALG_SetOpponentIndex(int iIndex, int iOpponentNr) {
        _opponentIndex = iIndex;
    }
    virtual void ALG_SetAssociateIndex(int iIndex) {}
    virtual void ALG_NewMatch(int iNumPlayer) {}
    virtual void ALG_NewGiocata(const CARDINFO* pCardArray, int iNumOfCards,
                                int iPlayerIx);
    virtual void ALG_PlayerHasPlayed(int iPlayerIx, const CARDINFO* pCard);
    virtual void ALG_PlayerHasSaid(int iPlayerIx, eSayPlayer eSay);
    virtual void ALG_PlayerHasVadoDentro(int iPlayerIx);
    virtual void ALG_Play();
    virtual void ALG_ManoEnd(I_MatchScore* pScore);
    virtual void ALG_GiocataEnd(I_MatchScore* pScore);
    virtual void ALG_MatchEnd(I_MatchScore* pScore);
    virtual void ALG_HaveToRespond();
    virtual void ALG_GicataScoreChange(eGiocataScoreState eNewScore);
    virtual void ALG_PlayerSaidWrong(int iPlayerIx);

   private:
    int animateCards();
    void drawStaticScene();
    int initDeck();
    int loadCardPac();
    void createRegionsInit();
    void renderCard(CardGfx* pCard);
    void drawPlayedCard(CardGfx* pCard);

    void handleKeyDownEvent(SDL_Event& event);
    void handleMouseDownEvent(SDL_Event& event);
    void handleMouseMoveEvent(SDL_Event& event);
    void handleMouseUpEvent(SDL_Event& event);
    void clickOnPlayerCard(int iIndex);
    void vadoDentro(int cardIx);
    void renderScreen();
    void drawVadoDentroCard(CardGfx* pCard);
    void showPlayerMarkup(int iPlayerIx);
    void showManoScore(bool bIsPlayed, int iPlayerIx, bool bIsPata,
                       int iManoNum);
    void guiPlayerTurn(int iPlayer);
    void showCurrentScore();
    void showPointsPlayer(int iPlayerIx, VCT_INT& vct_Points);
    void animGiocataEnd(int iPlayerIx, bool bIsPata);
    void enableCmds();
    void setCmdButton(size_t iButtonIndex, eSayPlayer eSay, LPCSTR strCaption);
    void enableOnlyCmdButtons(size_t iNumButt);
    void renderPlayerName(int iPlayerIx);
    void cleanup();
    void animateBeginGiocata();
    void showOkMsgBox(LPCSTR strText);
    int showYesNoMsgBox(LPCSTR strText);
    void animateManoEnd(int iPlayerIx);
    void showPopUpCallMenu(CardSpec& cardClicked, int iX, int iY,
                           eSayPlayer* peSay);
    void opponentHasPlayedCard(CardSpec& Card, bool vadoDentro);

   private:
    SDL_Surface* _p_Screen;
    SDL_Renderer* _p_sdlRenderer;
    SDL_Texture* _p_ScreenTexture;
    SDL_Surface* _p_Scene_background;
    SDL_Surface* _p_AlphaDisplay;
    SDL_Surface* _p_Deck;
    SDL_Surface* _p_Symbols;
    SDL_Rect _SrcBack;
    SDL_Rect _SrcCard;
    DeckType* _p_DeckType;
    TTF_Font* _p_FontStatus;
    TTF_Font* _p_FontText;
    SDL_Surface* _p_Surf_Bar;
    InvidoCore* _p_InvidoCore;
    int _cardWidth;
    int _cardHeight;
    int _symbolWidth;
    int _symbolHeigth;
    CardGfx _aPlayerCards[NUM_CARDS_HAND];
    CardGfx _aOpponentCards[NUM_CARDS_HAND];
    CardGfx _CardsTakenPla;
    CardGfx _CardsTakenOpp;
    CardGfx _CardsTable[NUM_CARDS_PLAYED];
    SDL_Surface* _p_AnImages[NUM_ANIMAGES];
    int _playerGuiIndex;
    int _opponentIndex;
    int _cardVadoDentroIndex;
    MAP_SCORE _MapPunti;
    MAP_SAY _Map_bt_Say;
    MAP_SAY _Map_fb_Say;
    MAP_INTID _Map_id_EchoSay;
    MAP_INTID _Map_idSynth_Say;
    MatchPoints* _p_MatchPoints;
    bool _isPlayerCanPlay;
    int _playerThatHaveMarkup;
    ButtonGfx* _p_btArrayCmd[NUMOFBUTTON];
    eSayPlayer _CmdDet[NUMOFBUTTON];
    BalloonGfx* _p_balGfx;
    Languages* _p_LangMgr;
    MusicManager* _p_MusicMgr;
    bool _isMatchTerminated;
};

#endif
