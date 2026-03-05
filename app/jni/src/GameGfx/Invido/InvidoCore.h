

// InvidoCoreEngine.h

#ifndef _C_INVIDOCORE_H_
#define _C_INVIDOCORE_H_

#include "Giocata.h"
#include "InvidoCoreDef.h"
#include "Mano.h"
#include "MatchPoints.h"
#include "Partita.h"
#include "PlayerStatistic.h"
#include "PlayersOnTable.h"

namespace invido {

class Mazzo;
class PlayersOnTable;

class InvidoCore : public I_CORE_Game {
   public:
    InvidoCore();
    virtual ~InvidoCore();

    void Init(PlayersOnTable* pPlayersOnTable, Mazzo* pMazzo);
    void AddPlayer(Player& player);
    Player* GetPlayerInPlaying() { return _p_PlHaveToPlay; };
    void SetGameType(eGameType eVal) { _eGameType = eVal; }
    Player* GetPlayer(Uint8 index);
    int GetNumOfPlayers() { return _numPlayers; }
    MatchPoints* GetMatchPointsInstance() { return &_matchPoints; }
    PlayersOnTable* GetTable() { return _p_PlayersOnTable; }

    // functions related to the script engine
    void NotifyScript(eScriptNotification eVal);
    void Script_OverrideDeck(Uint8 playerIx, int iC1, int iC2, int iC3);
    void Script_Say(Uint8 playerIx, eSayPlayer eSay);
    void Script_Play(Uint8 playerIx, CardSpec& CardPlayed);
    void NotifyScriptAlgorithm(Uint8 playerIx, eScriptNotification eVal);
    void Script_MatchEnd();
    
    // functions called from Mano
    void Mano_End();
    void NtyWaitingPlayer_Toplay(Uint8 playerIx);
    void NtyWaitingPlayer_ToResp(Uint8 playerIx);
    void NtyPlayerSayBuiada(Uint8 playerIx);
    void ChangeGiocataScore(eGiocataScoreState eNewScore);
    void Player_VaVia(Uint8 playerIx);
    void Giocata_AMonte();

    void Giocata_End();
    void Giocata_Start(Uint8 playerIx);

    void Partita_End();

    void NewMatch();
    void NextAction();
    void SetRandomSeed(int iVal);
    // Interface I_CORE_Game
    bool Say(Uint8 playerIx, eSayPlayer eSay);
    bool PlayCard(Uint8 playerIx, const CardSpec& cardSpec);
    bool VaDentro(Uint8 playerIx, const CardSpec& cardSpec);
    void AbandonGame(Uint8 playerIx);
    void GetAdmittedCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIxIndex);
    void GetMoreCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIxIndex);
    

   private:
    CardSpec* checkValidCardPlayed(Uint8 playerIx, const CardSpec& cardSpec);
    void resetCardInfoPlayers();
    bool resetCard(Uint8 playerIx, const CardSpec& cardSpec);
    Uint8 getNewMatchFirstPlayer();

   private:
    PlayersOnTable* _p_PlayersOnTable;
    Mazzo* _p_MyMazzo;
    eGameType _eGameType;
    size_t _numPlayers;
    Player* _p_PlHaveToPlay;
    Player* _p_StartPlayer;
    eGameLevel _eGameLevel;
    MATCH_STATISTIC _matchStat;
    Partita _partita;
    Giocata _giocata;
    Mano _mano;
    I_ALG_Player* _vctpAlgPlayer[MAX_NUM_PLAYER];
    MatchPoints _matchPoints;
    VCT_CARDSPEC _cardInfos[MAX_NUM_PLAYER];
};

}  // namespace invido

#endif