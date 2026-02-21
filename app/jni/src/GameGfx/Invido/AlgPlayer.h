#ifndef _ALG_PLAYER_H__
#define _ALG_PLAYER_H__

#include "AlgCoreInterface.h"
#include "CardSpec.h"
#include "InvidoCoreDef.h"

namespace invido {

class AlgPlayer : public I_ALG_Player {
   public:
    AlgPlayer();
    virtual ~AlgPlayer();

    // interface I_ALG_Player - start
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore);
    virtual void ALG_SetPlayerIndex(Uint8 playerIx);
    virtual void ALG_SetOpponentIndex(Uint8 playerIx);
    virtual void ALG_SetAssociateIndex(Uint8 playerIx);
    virtual void ALG_NewMatch(int iNumPlayer);
    virtual void ALG_NewGiocata(const VCT_CARDSPEC& vctCards, Uint8 playerIx);
    virtual void ALG_PlayerHasPlayed(Uint8 playerIx, const CardSpec& cardSpec);
    virtual void ALG_PlayerHasVadoDentro(Uint8 playerIx);
    virtual void ALG_PlayerHasSaid(Uint8 playerIx, eSayPlayer eSay);
    virtual void ALG_ManoEnd(I_MatchScore* pScore);
    virtual void ALG_GiocataEnd(I_MatchScore* pScore);
    virtual void ALG_MatchEnd(I_MatchScore* pScore);
    virtual void ALG_HaveToPlay();
    virtual void ALG_HaveToRespond();
    virtual void ALG_GicataScoreChange(eGiocataScoreState eNewScore);
    virtual void ALG_PlayerSaidWrong(Uint8 playerIx);
    // interface I_ALG_Player - end

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
    std::vector<CardSpec> m_vct_Cards_CPU;
    std::vector<CardSpec> m_vct_Cards_played[NUM_HANDS];
    eGiocataScoreState m_eScoreCurrent;
    I_CORE_Game* m_pCoreGame;
    int m_iMyIndex;
    int m_iOppIndex;
    eSayPlayer m_OpponentSay;
    eSayPlayer m_sayOppRisp;
    eSayPlayer m_MyLastSay;
    eSayPlayer m_sayMyRisp;
    int m_arrIxPlayerWonHand[NUM_CARDS_HAND];
    bool m_bLastManoPatada;
    int m_iNumChiamateMonte;
    int m_iNumManiWon;
    int m_iPlayerOnTurn;
    int m_iNumChiamateInGiocata;
    bool m_bIamCalledPoints;
    int m_iCPUCardDentroPos;
    bool m_opponetIsVadoDentro;
    bool m_WonFirstHand;
    int m_ixCurrMano;
};

}  // namespace invido

#endif
