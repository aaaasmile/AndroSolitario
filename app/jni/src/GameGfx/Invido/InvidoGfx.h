

// InvidoGfx.h

#ifndef __CINVIDO_GFX_H
#define __CINVIDO_GFX_H

#include <SDL_ttf.h>

#include <map>

#include "AlgPlayerInterface.h"
#include "CardGfx.h"
#include "InvidoCore.h"
enum {
    CRD_MAZZOALTRECARTE = 0,
    CRD_PLAYER1,
    CRD_PLAYER2,
    CRD_TAVOLO_1,
    CRD_TAVOLO_2,
    CRD_MAZZO_PLAYER1,
    CRD_MAZZO_PLAYER2,
};
struct DRAGCARD {
    int x;
    int y;
    int width;
    int height;
};

// card sybols
const unsigned int CRD_NSYMBOL = 0;
const unsigned int CRD_XSYMBOL = 1;
const unsigned int CRD_OSYMBOL = 2;
const unsigned int CRD_HSYMBOL = 3;

class AppGfx;
class InvidoCore;
class TipoDiMazzo;
class ButtonGfx;
class BalloonGfx;
class Languages;
class MusicManager;

typedef std::map<eGiocataScoreState, STRING> MAP_SCORE;
typedef std::map<eSayPlayer, STRING> MAP_SAY;
typedef std::map<eSayPlayer, int> MAP_INTID;
typedef std::vector<int> VCT_INT;
/**
// class to handle the invido game grafic during the match.
*/
class InvidoGfx : public I_ALG_Player {
   public:
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

    InvidoGfx(AppGfx* pApp);
    ~InvidoGfx();
    void Initialize(SDL_Surface* s, SDL_Renderer* pRender,
                    SDL_Texture* pScreenTexture);
    void MatchLoop();
    void InitInvidoVsCPU();
    void SetMainApp(AppGfx* pVal) { m_pApp = pVal; }
    void INP_PlayerSay(eSayPlayer eSay);
    void ButCmdClicked(int iButID);
    void NtfyTermEff(int iCh);
    Player* GetPlayer(int iIndex) { return m_pInvidoCore->GetPlayer(iIndex); }

    // Interface I_ALG_Player (callback functions from core invido engine)
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore) {}
    virtual void ALG_SetPlayerIndex(int iIndex) { m_PlayerGuiIndex = iIndex; }
    virtual void ALG_SetOpponentIndex(int iIndex, int iOpponentNr) {
        m_iOpponentIndex = iIndex;
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
    SDL_Surface* m_pScreen;
    SDL_Renderer* m_psdlRenderer;
    SDL_Texture* m_pScreenTexture;
    SDL_Surface* m_pScene_background;
    SDL_Surface* m_pAlphaDisplay;
    SDL_Surface* m_pDeck;
    SDL_Surface* m_pSymbols;
    SDL_Rect m_SrcBack;
    SDL_Rect m_SrcCard;
    TipoDiMazzo* m_pDeckType;
    TTF_Font* m_pFontStatus;
    TTF_Font* m_pFontText;
    SDL_Surface* m_pSurf_Bar;
    InvidoCore* m_pInvidoCore;
    AppGfx* m_pApp;
    bool m_bStartdrag;
    int m_iCardWidth;
    int m_iCardHeight;
    int m_iSymbolWidth;
    int m_iSymbolHeigth;
    CardGfx m_aPlayerCards[NUM_CARDS_HAND];
    CardGfx m_aOpponentCards[NUM_CARDS_HAND];
    CardGfx m_CardsTakenPla;
    CardGfx m_CardsTakenOpp;
    CardGfx m_CardsTable[NUM_CARDS_PLAYED];
    SDL_Surface* m_pAnImages[NUM_ANIMAGES];
    int m_PlayerGuiIndex;
    int m_iOpponentIndex;
    int m_CardVadoDentroIndex;
    MAP_SCORE m_MapPunti;
    MAP_SAY m_Map_bt_Say;
    MAP_SAY m_Map_fb_Say;
    MAP_INTID m_Map_id_EchoSay;
    MAP_INTID m_Map_idSynth_Say;
    MatchPoints* m_pMatchPoints;
    bool m_bPlayerCanPlay;
    int m_iPlayerThatHaveMarkup;
    DelayNextAction m_DelayAction;
    ButtonGfx* m_pbtArrayCmd[NUMOFBUTTON];
    eSayPlayer m_CmdDet[NUMOFBUTTON];
    BalloonGfx* m_pbalGfx;
    Languages* m_pLangMgr;
    MusicManager* m_pMusicMgr;
    bool m_bMatchTerminated;
};

#endif  // CGAME_H
