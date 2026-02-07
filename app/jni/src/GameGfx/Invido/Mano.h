#ifndef ____CMANO_H
#define ____CMANO_H

#include <deque>
#include <map>

#include "Config.h"
#include "InvidoCoreEnv.h"

class InvidoCore;
class Giocata;
class MatchPoints;
enum eFN_MANOACTION {
    MANO_WAITPL_TOPLAY = 0,
    MANO_WAIPL_TORESP,
    MANO_SAYBUIDA,
    MANO_AMONTE,
    MANO_VADODENTRO,
    MANO_VADOVIA,
    MANO_END,
    MANO_CHANGESCORE,
    MANO_NO_ACTION
};

typedef std::map<eSayPlayer, eGiocataScoreState> MAP_SAY_SCORE;
typedef std::map<eGiocataScoreState, eSayPlayer> MAP_SCORE_SAY;
typedef std::map<eFN_MANOACTION, STRING> MAP_ACTION_NAMES;
typedef std::map<int, eManoStatus> MAP_PL_STATUS;
typedef std::map<eManoStatus, int> MAP_STATUS_PL;
typedef std::map<eManoStatus, eFN_MANOACTION> MAP_STATUS_ACTION;
typedef std::map<eGiocataScoreState, eGiocataScoreState> MAP_SCORE_SCORENEXT;
typedef std::deque<eManoStatus> DEQ_TABLESTATE;

class PendQuestion {
   public:
    PendQuestion() {
        _isAMonte = false;
        _eScore = SC_CANELA;
        _playerIx = NOT_VALID_INDEX;
    }
    PendQuestion(bool bVal, eGiocataScoreState eSc, int iPl) {
        _isAMonte = bVal;
        _eScore = eSc;
        _playerIx = iPl;
    }
    void operator=(const PendQuestion& r);
    eGiocataScoreState _eScore;
    bool _isAMonte;
    int _playerIx;
};

typedef std::deque<PendQuestion> DEQ_PENDQUESTION;

//////////////////////////////////////////////////////////////////////

#ifndef VCT_INT
typedef std::vector<int> VCT_INT;
#endif

class ActionItem {
   public:
    ActionItem() { _eNextAction = MANO_NO_ACTION; }

    eFN_MANOACTION _eNextAction;
    VCT_INT _vct_iArg;
};

typedef std::deque<ActionItem> DEQ_ACTIONITEM;

class PlayersOnTable;
class TraceService;
class Mano {
   public:
    Mano();
    void SetCore(InvidoCore* pVal) { _p_InvidoCore = pVal; }
    void SetGiocata(Giocata* pVal) { _p_Giocata = pVal; }
    void SetScore(MatchPoints* pVal) { _p_Score = pVal; }
    void NewMano(int iPlayerIx);
    bool Player_Say(int iPlayerIx, eSayPlayer eSay);
    // player play a card
    bool Player_Play(int iPlayerIx, bool vadoDentro);
    void Reset();
    void NextAction();
    eManoStatus GetState() { return _eManoState; }
    void MatchStart();
    void GetAdmittedCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex);
    void GetMoreCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex);
    void CommandWithPendingQuestion(PendQuestion& PendQues,
                                    VCT_COMMANDS& vct_Commands,
                                    int iPlayerIndex);
    void GiocataStart();

   private:
    void actionOnQuestion(PendQuestion& PendQues);
    void handleVadoVia(int iPlayerIx);
    void handleVaBene(int iPlayerIx);
    void handle_ScoreCalled(int iPlayerIx, eSayPlayer eSay);
    void handle_MonteCall(int iPlayerIx, eSayPlayer eSay);
    void handle_CallMoreOrInvido(int iPlayerIx);
    void handle_CallNo(int iPlayerIx);
    void add_Action(int iPlayerIx, eFN_MANOACTION eAct);
    bool get_LastPendQuest(PendQuestion& PendQues);
    void add_QuestMonte(int iPlayerIx);
    void remove_LastQuestion();
    void restore_StateBeforeQuest();
    void clearQuestions();
    void save_StateBeforeQuest();
    bool IsPlayerOnCardPl();
    eManoStatus nextTableState();
    bool nextAvailSayScore(eSayPlayer* peSayAvail);
    bool isScoreBigClosed(eGiocataScoreState eS1, eGiocataScoreState eS2);
    void actionWithoutQuestion();
    void giocata_Go_Amonte(int iPlayerIx);
    bool isGiocataAMonte();
    bool get_LastPendScoreQuest(PendQuestion& PendQues);
    void removeObsoleteActions();

   private:
    InvidoCore* _p_InvidoCore;
    Giocata* _p_Giocata;
    eManoStatus _eManoState;
    eManoStatus _eOldManoState;
    eTypeOfPLayer _eTypePlayer[MAX_NUM_PLAYER];
    int _numOfPlayers;
    DEQ_ACTIONITEM _deqNextAction;
    MatchPoints* _p_Score;
    DEQ_PENDQUESTION _deqPendQuestion;
    MAP_ACTION_NAMES _MapActionNames;
    MAP_SAY_SCORE _MapSayScore;
    MAP_PL_STATUS _MapManoStatePl;
    MAP_PL_STATUS _MapManoStateResp;
    MAP_STATUS_PL _MapPlayerOnState;
    MAP_STATUS_ACTION _MapActionOnState;
    DEQ_TABLESTATE _deqTableState;
    PlayersOnTable* _p_Table;
    int _playerChangeScore;
    MAP_SCORE_SCORENEXT _mapScoreScNext;
    MAP_SCORE_SAY _mapScoreSay;
};

#endif