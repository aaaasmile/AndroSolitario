#ifndef ____CMANO_H
#define ____CMANO_H

#include <deque>
#include <map>

#include "AlgPlayerInterface.h"
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

//////////////////////////////////////////////////////////////////////
/** pendi question information
 */
class PendQuestion {
   public:
    PendQuestion() {
        m_bIsAMonte = false;
        m_eScore = SC_CANELA;
        m_iPlayerIx = NOT_VALID_INDEX;
    }
    PendQuestion(bool bVal, eGiocataScoreState eSc, int iPl) {
        m_bIsAMonte = bVal;
        m_eScore = eSc;
        m_iPlayerIx = iPl;
    }
    void operator=(const PendQuestion& r);
    eGiocataScoreState m_eScore;
    bool m_bIsAMonte;
    int m_iPlayerIx;
};

typedef std::deque<PendQuestion> DEQ_PENDQUESTION;

//////////////////////////////////////////////////////////////////////

#ifndef VCT_INT
typedef std::vector<int> VCT_INT;
#endif
/** action item in the queue
 */
class ActionItem {
   public:
    ActionItem() { m_eNextAction = MANO_NO_ACTION; }

    eFN_MANOACTION m_eNextAction;
    VCT_INT m_vct_iArg;
};

typedef std::deque<ActionItem> DEQ_ACTIONITEM;

class PlayersOnTable;
class TraceService;
class Mano {
   public:
    Mano();
    void SetCore(InvidoCore* pVal) { m_pInvidoCore = pVal; }
    void SetGiocata(Giocata* pVal) { m_pGiocata = pVal; }
    void SetScore(MatchPoints* pVal) { m_pScore = pVal; }
    void NewMano(int iPlayerIx);
    bool Player_Say(int iPlayerIx, eSayPlayer eSay);
    // player play a card
    bool Player_Play(int iPlayerIx, bool vadoDentro);
    void Reset();
    void NextAction();
    eManoStatus GetState() { return m_eManoState; }
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
    InvidoCore* m_pInvidoCore;
    Giocata* m_pGiocata;
    eManoStatus m_eManoState;
    eManoStatus m_eOldManoState;
    eTypeOfPLayer m_eTypePlayer[MAX_NUM_PLAYER];
    int m_iNumOfPlayers;
    DEQ_ACTIONITEM m_deqNextAction;
    MatchPoints* m_pScore;
    DEQ_PENDQUESTION m_deqPendQuestion;
    MAP_ACTION_NAMES m_MapActionNames;
    MAP_SAY_SCORE m_MapSayScore;
    MAP_PL_STATUS m_MapManoStatePl;
    MAP_PL_STATUS m_MapManoStateResp;
    MAP_STATUS_PL m_MapPlayerOnState;
    MAP_STATUS_ACTION m_MapActionOnState;
    DEQ_TABLESTATE m_deqTableState;
    PlayersOnTable* m_pTable;
    int m_iPlayerChangeScore;
    MAP_SCORE_SCORENEXT m_mapScoreScNext;
    MAP_SCORE_SAY m_mapScoreSay;
    TraceService* m_pTracer;
};

#endif