

// InvidoCoreEngine.h

#ifndef _C_INVIDOCORE_H_
#define _C_INVIDOCORE_H_

#include "ErrorInfo.h"
#include "GameActionRecorder.h"
#include "Giocata.h"
#include "InvidoCoreDef.h"
#include "Mano.h"
#include "MatchStateWithScore.h"
#include "Partita.h"
#include "PlayerStatistic.h"
#include "PlayersOnTable.h"

namespace invido {

class GameActionReplayer;
class Mazzo;
class PlayersOnTable;

class InvidoCore : public I_CORE_Game {
   public:
    InvidoCore();
    ~InvidoCore();

    void Init(PlayersOnTable* pPlayersOnTable, Mazzo* pMazzo);
    Player* GetPlayerInPlaying() { return _p_PlHaveToPlay; };
    void SetGameType(eGameType eVal) { _eGameType = eVal; }
    Player* GetPlayer(Uint8 index);
    int GetNumOfPlayers() { return _numPlayers; }
    MatchStateWithScore* GetMatchStateWithScoreInstance() { return &_matchStateWithScore; }
    PlayersOnTable* GetTable() { return _p_PlayersOnTable; }
    bool IsMatchOnGoing() { return _partita.IsOngoing(); }

    // functions related to the script engine
    void NotifyScript(eScriptNotification eVal);
    void Script_OverrideDeck(Uint8 playerIx, int iC1, int iC2, int iC3);
    void Script_Say(Uint8 playerIx, eSayPlayer eSay);
    void Script_Play(Uint8 playerIx, CardSpec& CardPlayed);
    void NotifyScriptAlgorithm(Uint8 playerIx, eScriptNotification eVal);
    void Script_MatchEnd();

    // functions called from Mano
    void Mano_End();
    LPErrInApp NtyWaitingPlayer_Toplay(Uint8 playerIx);
    LPErrInApp NtyWaitingPlayer_ToResp(Uint8 playerIx);
    void NtyPlayerSayBuiada(Uint8 playerIx);
    void ChangeGiocataScore(eGiocataScoreState eNewScore);
    void Player_VaVia(Uint8 playerIx);
    void Giocata_AMonte();
    void RecordEngineAction(const char* actionName, int parameter);
    void RecordGiocataAction(const char* actionName, int playerIx);
    void RecordPartitaAction(const char* actionName, int playerIx);

    void Giocata_End();
    void Giocata_Start(Uint8 playerIx);

    // Partita callbacks
    void PartitaEnded();
    void PartitaStarted(Uint8 playerIx);

    void NewMatch();
    LPErrInApp NextAction(bool& hasAction);
    void SetRandomSeed(int iVal);
    void SetRecordingLocation(const std::string& directory,
                              const std::string& gameName);
    void SetGameActionReplayer(GameActionReplayer& replayer);
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
    PartitaCb prepPartitaDelegator();
    GiocataCb prepGiocataDelegator();
    ManoCb prepManoDelegator();

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
    MatchStateWithScore _matchStateWithScore;
    GameActionRecorder _gameActionRecorder;
    GameActionReplayer* _p_GameActionReplayer;
    std::string _recordingDirectory;
    std::string _gameName;
    std::vector<I_ALG_Player*> _vctpAlgPlayer;
    std::vector<VCT_CARDSPEC> _vctCardInfos;
};

}  // namespace invido

#endif