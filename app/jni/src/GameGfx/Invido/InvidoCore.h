

// InvidoCoreEngine.h

#ifndef _C_INVIDOCORE_H_
#define _C_INVIDOCORE_H_

#include "Giocata.h"
#include "InvidoCoreEnv.h"
#include "Mano.h"
#include "MatchPoints.h"
#include "Partita.h"
#include "PlayerStatistic.h"
#include "PlayersOnTable.h"

class Mazzo;

class InvidoCore : public I_CORE_Game {
   public:
    InvidoCore();
    ~InvidoCore();
    void Create(Player* pHmiPlayer, int iNumPlayers);
    bool WhoWonsTheGame(Player** ppPlayer);
    bool GetPlayerInPlaying(Player** ppPlayer);
    void SetGameType(eGameType eVal) { _eGameType = eVal; }
    Player* GetPlayer(int iIndex) {
        return _playersOnTable.GetPlayerIndex(iIndex);
    }
    void RaiseError(const std::string& errorMsg);
    int GetNumOfPlayers() { return _numPlayers; }
    MatchPoints* GetMatchPointsObj() { return &_matchPoints; }
    PlayersOnTable* GetTable() { return &_playersOnTable; }

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

    void Giocata_End();
    void Giocata_Start(long lPlayerIx);

    void Partita_End();

    void NewMatch();
    void NextAction();
    void SetRandomSeed(int iVal);

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
    PlayersOnTable _playersOnTable;
    eGameType _eGameType;
    long _numPlayers;
    Player* _p_PlHaveToPlay;
    Player* _p_StartPlayer;
    eGameLevel _eGameLevel;
    MATCH_STATISTIC _matchStat;
    Mazzo* _p_MyMazzo;
    Partita _partita;
    Giocata _giocata;
    Mano _mano;
    I_ALG_Player* _vctpAlgPlayer[MAX_NUM_PLAYER];
    MatchPoints _matchPoints;
    CardSpec _cardInfos[NUM_CARDS_HAND * MAX_NUM_PLAYER];
};

#endif