#ifndef ____CMANO_H
#define ____CMANO_H

#include <deque>
#include <map>

#include "Config.h"
#include "ErrorInfo.h"
#include "InvidoCoreDef.h"

namespace invido {

class InvidoCore;
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
typedef std::map<Uint8, eManoStatus> MAP_PL_STATUS;
typedef std::map<eManoStatus, Uint8> MAP_STATUS_PL;
typedef std::map<eManoStatus, eFN_MANOACTION> MAP_STATUS_ACTION;
typedef std::map<eGiocataScoreState, eGiocataScoreState> MAP_SCORE_SCORENEXT;
typedef std::deque<eManoStatus> DEQ_TABLESTATE;

struct PendingQuestion {
    PendingQuestion() {
        isAMonte = false;
        eScore = SC_CANELA;
        playerIx = 0;
    }
    PendingQuestion(bool bVal, eGiocataScoreState eSc, Uint8 plIx) {
        isAMonte = bVal;
        eScore = eSc;
        playerIx = plIx;
    }
    void operator=(const PendingQuestion& r);
    eGiocataScoreState eScore;
    bool isAMonte;
    Uint8 playerIx;
};

typedef std::deque<PendingQuestion> DEQ_PENDQUESTION;

//////////////////////////////////////////////////////////////////////

#ifndef VCT_INT
typedef std::vector<int> VCT_INT;
#endif

class PlayersOnTable;
class TraceService;
class Mano {
    struct ActionItemMano {
        ActionItemMano() { _eNextAction = MANO_NO_ACTION; }

        eFN_MANOACTION _eNextAction;
        VCT_INT _vctArg;
    };

    typedef std::deque<ActionItemMano> DEQ_ACTIONITEM;

   public:
    Mano();
    void SetCore(InvidoCore* pVal) { _p_InvidoCore = pVal; }
    void SetScore(MatchPoints* pVal) { _p_Score = pVal; }
    LPErrInApp NewMano(Uint8 playerIx);
    bool Player_Say(Uint8 playerIx, eSayPlayer eSay);
    // player play a card
    bool Player_Play(Uint8 playerIx, bool vadoDentro);
    void Reset();
    void NextAction();
    eManoStatus GetState() { return _eManoState; }
    void MatchStart(int numPlayers);
    void GetAdmittedCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIx);
    void GetMoreCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIx);
    void CommandWithPendingQuestion(PendingQuestion& PendQues,
                                    VCT_COMMANDS& vct_Commands, Uint8 playerIx);
    void GiocataStart();

   private:
    Uint8 getNextPlayerIxAfter(Uint8 playerIx);
    void actionOnQuestion(PendingQuestion& PendQues);
    void handleVadoVia(Uint8 playerIx);
    void handleVaBene(Uint8 playerIx);
    void handle_ScoreCalled(Uint8 playerIx, eSayPlayer eSay);
    void handle_MonteCall(Uint8 playerIx, eSayPlayer eSay);
    void handle_CallMoreOrInvido(Uint8 playerIx);
    void handle_CallNo(Uint8 playerIx);
    void add_Action(int iPar_0, eFN_MANOACTION eAct);
    bool get_LastPendQuest(PendingQuestion& PendQues);
    void add_QuestMonte(Uint8 playerIx);
    void remove_LastQuestion();
    void restore_StateBeforeQuest();
    void clearQuestions();
    void save_StateBeforeQuest();
    bool IsPlayerOnCardPl();
    eManoStatus nextTableState();
    bool nextAvailSayScore(eSayPlayer* peSayAvail);
    bool isScoreBigClosed(eGiocataScoreState eS1, eGiocataScoreState eS2);
    void actionWithoutQuestion();
    void giocata_Go_Amonte(Uint8 playerIx);
    bool isGiocataAMonte();
    bool get_LastPendScoreQuest(PendingQuestion& PendQues);
    void removeObsoleteActions();

   private:
    InvidoCore* _p_InvidoCore;
    eManoStatus _eManoState;
    eManoStatus _eOldManoState;
    Uint8 _numOfPlayers;
    DEQ_ACTIONITEM _deqNextAction;
    MatchPoints* _p_Score;
    DEQ_PENDQUESTION _deqPendingQuestion;
    MAP_ACTION_NAMES _mapActionNames;
    MAP_SAY_SCORE _mapSayScore;
    MAP_PL_STATUS _mapManoStatePl;
    MAP_PL_STATUS _mapManoStateResp;
    MAP_STATUS_PL _mapPlayerOnState;
    MAP_STATUS_ACTION _mapActionOnState;
    DEQ_TABLESTATE _deqTableState;
    Uint8 _playerChangeScoreIx;
    MAP_SCORE_SCORENEXT _mapScoreScNext;
    MAP_SCORE_SAY _mapScoreSay;
};

}  // namespace invido

#endif