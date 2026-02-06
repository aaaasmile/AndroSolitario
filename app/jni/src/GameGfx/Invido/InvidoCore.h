

// InvidoCoreEngine.h

#ifndef _C_INVIDOCORE_H_
#define _C_INVIDOCORE_H_

#include "Giocata.h"
#include "InvidoCoreEnv.h"
#include "Mano.h"
#include "MatchPoints.h"
#include "Partita.h"
#include "PlayersOnTable.h"
#include "Probality.h"

class Mazzo;
class TraceService;

class InvidoCore : public I_CORE_Game {
   public:
    InvidoCore();
    ~InvidoCore();
    void Create(Player* pHmiPlayer, int iNumPlayers);
    bool WhoWonsTheGame(Player** ppPlayer);
    bool GetPlayerInPlaying(Player** ppPlayer);
    void SetGameType(eGameType eVal) { m_eGameType = eVal; }
    Player* GetPlayer(int iIndex) {
        return m_PlayersOnTable.GetPlayerIndex(iIndex);
    }
    void RaiseError(const std::string& errorMsg);
    int GetNumOfPlayers() { return m_lNumPlayers; }
    MatchPoints* GetMatchPointsObj() { return &m_MatchPoints; }
    PlayersOnTable* GetTable() { return &m_PlayersOnTable; }

    // functions related to the script engine
    void NotifyScript(eScriptNotification eVal);
    void Script_OverrideDeck(int iPlayer, int iC1, int iC2, int iC3);
    void Script_Say(int iPlayer, eSayPlayer eSay);
    void Script_Play(int iPlayer, CardSpec& CardPlayed);
    void NotifyScriptAlgorithm(int iPlayerIx, eScriptNotification eVal);
    void Script_MatchEnd();
    void Script_SetStartPlayer(int iPlayer);
    int Script_CheckResult(int iTypeOfItem, int iParam1, int iExpectedVal);

    // functions called from Mano
    void Mano_End();
    void NtyWaitingPlayer_Toplay(int iPlayerIx);
    void NtyWaitingPlayer_ToResp(int iPlayerIx);
    void NtyPlayerSayBuiada(int iPlayerIx);
    void ChangeGiocataScore(eGiocataScoreState eNewScore);
    void Player_VaVia(int iPlayerIx);
    void Giocata_AMonte();

    // functions called from Giocata
    void Giocata_End();
    void Giocata_Start(long lPlayerIx);

    // function called from Partita
    void Partita_End();

    // functions called from HMI
    void NewMatch();
    void NextAction();
    void SetRandomSeed(int iVal) { m_pMyMazzo->SetRandomSeed(iVal); }

    // I_CORE_Game: functions called from algorithm
    bool Player_saySomething(int iPlayerIx, eSayPlayer eSay);
    bool Player_playCard(int iPlayerIx, const CARDINFO* pCardInfo);
    bool Player_vaDentro(int iPlayerIx, const CARDINFO* pCardInfo);
    void GetAdmittedCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex);
    void GetMoreCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex);
    void AbandonGame(int iPlayerIx);

   private:
    CardSpec* isCardInPlayerHand(int iPlayerIx, const CARDINFO* pCardInfo);
    CardSpec* checkValidCardPlayed(int iPlayerIx, const CARDINFO* pCardInfo);
    void resetCardInfoPlayers();
    bool resetCard(int iPlayerIx, CARDINFO* pCardInfo);
    int getNewMatchFirstPlayer();

   private:
    PlayersOnTable m_PlayersOnTable;
    eGameType m_eGameType;
    long m_lNumPlayers;
    Player* m_pPlHaveToPlay;
    Player* m_pStartPlayer;
    eGameLevel m_eGameLevel;
    MATCH_STATISTIC m_Match_Stat;
    Mazzo* m_pMyMazzo;
    Partita m_Partita;
    Giocata m_Giocata;
    Mano m_Mano;
    I_ALG_Player* m_vctAlgPlayer[MAX_NUM_PLAYER];
    MatchPoints m_MatchPoints;
    CardSpec m_aCardInfo[NUM_CARDS_HAND * MAX_NUM_PLAYER];
    TraceService* m_pTracer;
};

#endif