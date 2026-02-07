#ifndef _ALG_PLAYER_H__
#define _ALG_PLAYER_H__

#include <functional>

#include "AlgPlayerInterface.h"
#include "CardSpec.h"
#include "InvidoCoreEnv.h"

class TraceService;
class cmpHandValue : public std::unary_function<cmpHandValue, bool> {
   private:
    int m_lValRef;

   public:
    // constructor
    explicit cmpHandValue() {}
    // operator ()
    bool operator()(const VCT_SINGLECARD& o1, const VCT_SINGLECARD& o2) {
        int iPoints2 = 0;
        int iPoints1 = 0;
        size_t iNumCards = o1.size();
        if (iPoints1 >= iPoints2) {
            // no swap
            return false;
        } else {
            // swap
            return true;
        }
    }
};
/** class that implement a dummy player
 */
class AlgAdvancedPlayer : public I_ALG_Player {
   public:
    AlgAdvancedPlayer();
    virtual ~AlgAdvancedPlayer();

    // interface I_ALG_Player
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore);
    virtual void ALG_SetPlayerIndex(int iIndex);
    virtual void ALG_SetOpponentIndex(int iIndex, int iOpponentNr);
    virtual void ALG_SetAssociateIndex(int iIndex);
    virtual void ALG_NewMatch(int iNumPlayer);
    virtual void ALG_NewGiocata(const CARDINFO* pCardArray, int iNumOfCards,
                                int iPlayerIx);
    virtual void ALG_PlayerHasPlayed(int iPlayerIx, const CARDINFO* pCard);
    virtual void ALG_PlayerHasVadoDentro(int iPlayerIx);
    virtual void ALG_PlayerHasSaid(int iPlayerIx, eSayPlayer eSay);
    virtual void ALG_Play();
    virtual void ALG_ManoEnd(I_MatchScore* pScore);
    virtual void ALG_GiocataEnd(I_MatchScore* pScore);
    virtual void ALG_MatchEnd(I_MatchScore* pScore);
    virtual void ALG_HaveToRespond();
    virtual void ALG_GicataScoreChange(eGiocataScoreState eNewScore);
    virtual void ALG_PlayerSaidWrong(int iPlayerIx);

   private:
    void PlayAsFirst();
    void PlayAsSecond();
    bool IsPlayerFirst();
    int NumMano();
    bool Cagna(int lastNumChiamate);
    bool ChiamaDiPiu(int lastNumChiamate);
    bool ChiamaAMonte(int lastNumChiamate);
    void Chiama(eSayPlayer eSay, int lastChiamataNum);
    void GiocaACaso();
    void doVadoDentro(int cardPos);
    void handleSayAmonte(int curr_mano, int pointsFirstCard,
                         int lastNumChiamate, int maxpoints, int sum_points);
    void handleSayPopints(int curr_mano, int pointsFirstCard,
                          int lastNumChiamate, int maxpoints, int sum_points);

   private:
    CardSpec m_vct_Cards_CPU[NUM_CARDS_HAND];
    std::vector<CardSpec> m_vct_Cards_played[NUM_HANDS];
    eGiocataScoreState m_eScoreCurrent;
    I_CORE_Game* m_pCoreGame;
    int m_iMyIndex;
    int m_iOppIndex;
    eSayPlayer m_OpponentSay;
    eSayPlayer m_sayOppRisp;
    eSayPlayer m_MyLastSay;
    eSayPlayer m_sayMyRisp;
    TraceService* m_pTracer;
    int m_arrIxPlayerWonHand[NUM_CARDS_HAND];
    bool m_bLastManoPatada;
    int m_iNumChiamateMonte;
    int m_iNumManiWon;
    int m_iPlayerOnTurn;
    int m_iNumChiamateInGiocata;
    int m_itrChan;
    bool m_bIamCalledPoints;
    int m_iCPUCardDentroPos;
    bool m_opponetIsVadoDentro;
    bool m_WonFirstHand;
    int m_ixCurrMano;
};

#endif
