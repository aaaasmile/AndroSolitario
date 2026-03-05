#include "Mano.h"

#include <cstddef>

#include "Config.h"
#include "InvidoCore.h"
#include "InvidoCoreDef.h"
#include "MatchPoints.h"
#include "TraceService.h"

using namespace invido;

static const char* stalpzActionName[10] = {
    "MANO_WAITPL_TOPLAY", "MANO_WAIPL_TORESP", "MANO_SAYBUIDA",
    "MANO_AMONTE",        "MANO_VADODENTRO",   "MANO_VADOVIA",
    "MANO_END",           "MANO_CHANGESCORE",  "MANO_NO_ACTION"};

static const char* stalpzManoState[10] = {
    "MNST_WAIT_NEW_MANO",      "MNST_WAIT_PLAY_PLAYER_1",
    "MNST_WAIT_PLAY_PLAYER_2", "MNST_WAIT_PLAY_PLAYER_3",
    "MNST_WAIT_PLAY_PLAYER_4", "MNST_MANO_END",
    "MNST_WAIT_RESP_PLAYER_1", "MNST_WAIT_RESP_PLAYER_2",
    "MNST_WAIT_RESP_PLAYER_3", "MNST_WAIT_RESP_PLAYER_4"};

static eGiocataScoreState intToEScore(int iVal) {
    eGiocataScoreState eVal = SC_PATTA;
    switch (iVal) {
        case 0:
            break;
        case 1:
            eVal = SC_CANELA;
            break;
        case 3:
            eVal = SC_INVIDO;
            break;
        case 6:
            eVal = SC_TRASMAS;
            break;
        case 9:
            eVal = SC_TRASMASNOEF;
            break;
        case 12:
            eVal = SC_FUERAJEUQ;
            break;
        case 24:
            eVal = SC_PARTIDA;
            break;

        default:
            SDL_assert(0);
    }
    return eVal;
}

void PendingQuestion::operator=(const PendingQuestion& r) {
    eScore = r.eScore;
    isAMonte = r.isAMonte;
    playerIx = r.playerIx;
}

Mano::Mano() {
    _p_InvidoCore = NULL;
    _eManoState = MNST_WAIT_NEW_MANO;
    _p_Score = NULL;

    // initialize map for action tracing
    _MapActionNames[MANO_WAITPL_TOPLAY] = "WAITPL_TOPLAY";
    _MapActionNames[MANO_WAIPL_TORESP] = "WAIPL_TORESP";
    _MapActionNames[MANO_SAYBUIDA] = "SAYBUIDA";
    _MapActionNames[MANO_AMONTE] = "AMONTE";
    _MapActionNames[MANO_VADODENTRO] = "VADODENTRO";
    _MapActionNames[MANO_VADOVIA] = "VADOVIA";
    _MapActionNames[MANO_END] = "END";
    _MapActionNames[MANO_CHANGESCORE] = "CHANGESCORE";
    _MapActionNames[MANO_NO_ACTION] = "NO_ACTION";

    // initialize map for conversion say and score
    _MapSayScore[eSayPlayer::SP_AMONTE] = SC_AMONTE;
    _MapSayScore[eSayPlayer::SP_INVIDO] = SC_INVIDO;
    _MapSayScore[eSayPlayer::SP_TRASMAS] = SC_TRASMAS;
    _MapSayScore[eSayPlayer::SP_TRASMASNOEF] = SC_TRASMASNOEF;
    _MapSayScore[eSayPlayer::SP_FUERAJEUQ] = SC_FUERAJEUQ;
    _MapSayScore[eSayPlayer::SP_PARTIDA] = SC_PARTIDA;

    // score say map
    _mapScoreSay[SC_AMONTE] = eSayPlayer::SP_AMONTE;
    _mapScoreSay[SC_INVIDO] = eSayPlayer::SP_INVIDO;
    _mapScoreSay[SC_TRASMAS] = eSayPlayer::SP_TRASMAS;
    _mapScoreSay[SC_TRASMASNOEF] = eSayPlayer::SP_TRASMASNOEF;
    _mapScoreSay[SC_FUERAJEUQ] = eSayPlayer::SP_FUERAJEUQ;
    _mapScoreSay[SC_PARTIDA] = eSayPlayer::SP_PARTIDA;

    // initilize map player status
    _MapManoStatePl[eGameConst::PLAYER1] = MNST_WAIT_PLAY_PLAYER_1;
    _MapManoStatePl[eGameConst::PLAYER2] = MNST_WAIT_PLAY_PLAYER_2;
    _MapManoStatePl[eGameConst::PLAYER3] = MNST_WAIT_PLAY_PLAYER_3;
    _MapManoStatePl[eGameConst::PLAYER4] = MNST_WAIT_PLAY_PLAYER_4;

    // responce status
    _MapManoStateResp[eGameConst::PLAYER1] = MNST_WAIT_RESP_PLAYER_1;
    _MapManoStateResp[eGameConst::PLAYER2] = MNST_WAIT_RESP_PLAYER_2;
    _MapManoStateResp[eGameConst::PLAYER3] = MNST_WAIT_RESP_PLAYER_3;
    _MapManoStateResp[eGameConst::PLAYER4] = MNST_WAIT_RESP_PLAYER_4;

    // player index map init
    _MapPlayerOnState[MNST_WAIT_PLAY_PLAYER_1] = eGameConst::PLAYER1;
    _MapPlayerOnState[MNST_WAIT_PLAY_PLAYER_2] = eGameConst::PLAYER2;
    _MapPlayerOnState[MNST_WAIT_PLAY_PLAYER_3] = eGameConst::PLAYER3;
    _MapPlayerOnState[MNST_WAIT_PLAY_PLAYER_4] = eGameConst::PLAYER4;

    // action on state init
    _MapActionOnState[MNST_WAIT_PLAY_PLAYER_1] = MANO_WAITPL_TOPLAY;
    _MapActionOnState[MNST_WAIT_PLAY_PLAYER_2] = MANO_WAITPL_TOPLAY;
    _MapActionOnState[MNST_WAIT_PLAY_PLAYER_3] = MANO_WAITPL_TOPLAY;
    _MapActionOnState[MNST_WAIT_PLAY_PLAYER_4] = MANO_WAITPL_TOPLAY;
    _MapActionOnState[MNST_MANO_END] = MANO_END;
    _MapActionOnState[MNST_WAIT_RESP_PLAYER_1] = MANO_WAIPL_TORESP;
    _MapActionOnState[MNST_WAIT_RESP_PLAYER_2] = MANO_WAIPL_TORESP;
    _MapActionOnState[MNST_WAIT_RESP_PLAYER_3] = MANO_WAIPL_TORESP;
    _MapActionOnState[MNST_WAIT_RESP_PLAYER_4] = MANO_WAIPL_TORESP;

    // score - score next
    _mapScoreScNext[eGiocataScoreState::SC_CANELA] =
        eGiocataScoreState::SC_INVIDO;
    _mapScoreScNext[eGiocataScoreState::SC_INVIDO] =
        eGiocataScoreState::SC_TRASMAS;
    _mapScoreScNext[eGiocataScoreState::SC_TRASMAS] =
        eGiocataScoreState::SC_TRASMASNOEF;
    _mapScoreScNext[eGiocataScoreState::SC_TRASMASNOEF] =
        eGiocataScoreState::SC_FUERAJEUQ;
    _mapScoreScNext[eGiocataScoreState::SC_FUERAJEUQ] =
        eGiocataScoreState::SC_PARTIDA;
}

void Mano::NextAction() {
    size_t iNumAct = _deqNextAction.size();
    if (iNumAct == 0) {
        // no action
        return;
    }
    ActionItem Action = _deqNextAction.front();
    _deqNextAction.pop_front();

    eGiocataScoreState eScore;
    TRACE_DEBUG("Process action (NextAction): %s\n",
                stalpzActionName[Action._eNextAction]);

    switch (Action._eNextAction) {
        case MANO_WAITPL_TOPLAY:
            SDL_assert(Action._vct_iArg.size() > 0);
            // may be _eNextAction is modified
            _p_InvidoCore->NtyWaitingPlayer_Toplay(Action._vct_iArg[0]);
            TRACE_DEBUG("Wait play player: %d\n", Action._vct_iArg[0]);
            break;

        case MANO_AMONTE:
            _p_InvidoCore->Giocata_AMonte();
            break;

        case MANO_VADODENTRO:
            // _p_InvidoCore->;
            //  TO DO
            SDL_assert(0);
            break;

        case MANO_VADOVIA:
            _p_InvidoCore->Player_VaVia(Action._vct_iArg[0]);
            break;

        case MANO_WAIPL_TORESP:
            _p_InvidoCore->NtyWaitingPlayer_ToResp(Action._vct_iArg[0]);
            break;

        case MANO_END:
            _p_InvidoCore->Mano_End();
            break;

        case MANO_CHANGESCORE:
            eScore = intToEScore(Action._vct_iArg[0]);
            _p_InvidoCore->ChangeGiocataScore(eScore);
            // process the next action without waiting the next trigger
            NextAction();
            break;

        case MANO_NO_ACTION:
            // nothing to do
            // process the next action without waiting the next trigger
            NextAction();
            break;

        case MANO_SAYBUIDA:
            _p_InvidoCore->NtyPlayerSayBuiada(Action._vct_iArg[0]);
            break;

        default:
            SDL_assert(0);
            break;
    }
}

void Mano::GiocataStart() {
    // reset player change score  index
    _playerChangeScore = NOT_VALID_INDEX;
}

void Mano::MatchStart(int numPlayers) { _numOfPlayers = numPlayers; }

LPErrInApp Mano::NewMano(Uint8 playerIx) {
    clearQuestions();

    if (_eManoState == MNST_WAIT_NEW_MANO || _eManoState == MNST_MANO_END) {
        SDL_assert(playerIx >= 0 && playerIx < _numOfPlayers);
        // set action
        add_Action(playerIx, MANO_WAITPL_TOPLAY);

        // prepare state queue on table players
        int aTablePlayer[MAX_NUM_PLAYER];

        // calculate the array of player indexes
        calcCircleIndex(aTablePlayer, MAX_NUM_PLAYER, playerIx);
        _deqTableState.clear();
        for (int i = 0; i < _numOfPlayers; i++) {
            int iTmpPlayer = aTablePlayer[i];
            eManoStatus eVal = _MapManoStatePl[iTmpPlayer];
            _deqTableState.push_back(eVal);
        }
        _deqTableState.push_back(MNST_MANO_END);

        // next state
        _eManoState = nextTableState();

    } else {
        return ERR_UTIL::ErrorCreate("Mano state is not correct %d",
                                     _eManoState);
    }
    return NULL;
}

bool Mano::Player_Play(Uint8 playerIx, bool vadoDentro) {
    TRACE_DEBUG("Player%d play state %s\n", playerIx,
                stalpzManoState[_eManoState]);

    if (_eManoState == MNST_WAIT_NEW_MANO || _eManoState == MNST_MANO_END) {
        // don't accept play if the mano is not initialized
        TRACE_DEBUG("Don't accept card play %d %s\n", playerIx,
                    stalpzManoState[_eManoState]);
        return false;
    }

    if (!IsPlayerOnCardPl()) {
        // status is not card playing: question is pending

        PendingQuestion PendQues;
        if (get_LastPendQuest(PendQues)) {
            if (PendQues.playerIx == playerIx) {
                // same player has make a question: waiting the responce before
                // play
                TRACE_DEBUG("Player %d, question is pending, wait responce\n",
                            playerIx);

                return false;
            }
            if (PendQues.isAMonte) {
                // a monte call is pending: play a card is not admitted
                TRACE_DEBUG("Player %d, monte call is pending, wait responce\n",
                            playerIx);
                return false;
            } else {
                // score question is pending: silent accept the question
                _p_Score->ChangeCurrentScore(PendQues.eScore,
                                             PendQues.playerIx);
                // clear question list
                clearQuestions();
                // save the player index that make a change
                _playerChangeScore = PendQues.playerIx;
            }
        } else {
            TRACE_DEBUG("no question and no player state. Wrong state\n");
            SDL_assert(0);
        }
    }

    eManoStatus eExpectedState = _MapManoStatePl[playerIx];
    if (eExpectedState != _eManoState) {
        // player is not allowed to play
        TRACE_DEBUG("Player %d, is not allowed to play, expected state %s\n",
                    playerIx, stalpzManoState[eExpectedState]);
        return false;
    }

    // next state
    _eManoState = nextTableState();
    // new player on play
    int iNewPlayer = _MapPlayerOnState[_eManoState];
    // new action
    eFN_MANOACTION eAct_Type = _MapActionOnState[_eManoState];

    add_Action(iNewPlayer, eAct_Type);
    return true;
}

bool Mano::Player_Say(Uint8 playerIx, eSayPlayer eSay) {
    if (_eManoState == MNST_WAIT_NEW_MANO || _eManoState == MNST_MANO_END) {
        // don't accept calls if the mano is not initialized
        return false;
    }

    bool bRes = true;

    if (eSay == eSayPlayer::SP_INVIDO || eSay == eSayPlayer::SP_TRASMAS ||
        eSay == eSayPlayer::SP_TRASMASNOEF ||
        eSay == eSayPlayer::SP_FUERAJEUQ || eSay == eSayPlayer::SP_PARTIDA) {
        handle_ScoreCalled(playerIx, eSay);
    } else if (eSay == SP_AMONTE) {
        // a monte request
        handle_MonteCall(playerIx, eSay);
    } else if (eSay == eSayPlayer::SP_VABENE || eSay == eSayPlayer::SP_GIOCA) {
        handleVaBene(playerIx);
    } else if (eSay == eSayPlayer::SP_VADOVIA) {
        handleVadoVia(playerIx);
    } else if (eSay == eSayPlayer::SP_CHIAMADIPIU ||
               eSay == eSayPlayer::SP_CHIAMA_BORTOLO) {
        handle_CallMoreOrInvido(playerIx);
    } else if (eSay == eSayPlayer::SP_NO) {
        handle_CallNo(playerIx);
    } else {
        SDL_assert(0);
    }

    return bRes;
}

bool Mano::isScoreBigClosed(eGiocataScoreState eS1, eGiocataScoreState eS2) {
    bool bRes = false;

    // eS2 is the smaller score
    eGiocataScoreState eTmp = _mapScoreScNext[eS2];
    if (eTmp == eS1) {
        // ok the 2 score are enought closed
        bRes = true;
    }
    return bRes;
}

void Mano::handle_ScoreCalled(Uint8 playerIx, eSayPlayer eSay) {
    int iAttScore = _p_Score->GetCurrScore();
    eGiocataScoreState eScore;
    eScore = _MapSayScore[eSay];

    PendingQuestion PendScoreQuesLast;
    PendingQuestion PendQuesMonte;
    if (get_LastPendScoreQuest(PendScoreQuesLast)) {
        // pending point question
        // score call is admitted only if the pending question  was a small of
        // the call score from opponent How small? only the closest call is
        // admitted (canela->invido->trasmas->noef->foera->partida)
        if (PendScoreQuesLast.playerIx != playerIx &&
            isScoreBigClosed(eScore, PendScoreQuesLast.eScore)) {
            // score call admitted

            // implicit change of score to the first call
            _p_Score->ChangeCurrentScore(PendScoreQuesLast.eScore,
                                         PendScoreQuesLast.playerIx);
            // save the player index that make a change
            _playerChangeScore = PendScoreQuesLast.playerIx;
            add_Action(PendScoreQuesLast.eScore, MANO_CHANGESCORE);

            // new question state
            PendingQuestion NewPendQues(false, eScore, playerIx);
            _deqPendingQuestion.push_back(NewPendQues);

            actionOnQuestion(NewPendQues);

        } else {
            // call not admitted
            add_Action(playerIx, MANO_SAYBUIDA);
            PendingQuestion PendTmp;
            if (get_LastPendQuest(PendTmp)) {
                // repeat trigger to restore the status before call
                actionOnQuestion(PendTmp);
            } else {
                SDL_assert(0);
            }
        }
    } else if (get_LastPendQuest(PendQuesMonte)) {
        // if a question is pending, it could be only a monte
        // a monte pending question
        SDL_assert(PendQuesMonte.isAMonte);
        if (playerIx == _playerChangeScore) {
            // the same player can't increment the score
            // call not admitted
            add_Action(playerIx, MANO_SAYBUIDA);
            actionOnQuestion(PendQuesMonte);
        }

    } else if (eScore > iAttScore) {
        if (playerIx == _playerChangeScore) {
            // the same player can't change the score
            add_Action(playerIx, MANO_SAYBUIDA);
            actionWithoutQuestion();
        } else {
            // no pending question, call accepted
            PendingQuestion NewPendQues(false, eScore, playerIx);
            _deqPendingQuestion.push_back(NewPendQues);

            actionOnQuestion(NewPendQues);
        }
    } else {
        // invalid callbecause eScore is to small
        add_Action(playerIx, MANO_SAYBUIDA);
        actionWithoutQuestion();
    }
}

void Mano::handle_MonteCall(Uint8 playerIx, eSayPlayer eSay) {
    PendingQuestion PendQuesLast;

    if (get_LastPendQuest(PendQuesLast)) {
        if (PendQuesLast.isAMonte) {
            // there is a MONTE pending question
            if (PendQuesLast.playerIx != playerIx) {
                // ok this giocata goes a monte
                giocata_Go_Amonte(playerIx);
            } else {
                // the same player call a monte, ignore it
            }
        } else {
            // insert the question
            add_QuestMonte(playerIx);
        }
    } else {
        // insert the question
        add_QuestMonte(playerIx);
    }
}

void Mano::giocata_Go_Amonte(Uint8 playerIx) {
    _p_Score->ChangeCurrentScore(SC_AMONTE, playerIx);
    add_Action(playerIx, MANO_AMONTE);
    _eManoState = MNST_WAIT_NEW_MANO;

    // clear question list
    clearQuestions();
}

void Mano::handle_CallMoreOrInvido(Uint8 playerIx) {
    // Nothing to do here
}

void Mano::handle_CallNo(Uint8 playerIx) {
    PendingQuestion PendQues;
    if (get_LastPendQuest(PendQues)) {
        // there is question pending, the responce is no
        // no is admitted only on "A monte" call
        if (PendQues.isAMonte) {
            // remove the monte question
            remove_LastQuestion();
            if (get_LastPendQuest(PendQues)) {
                // there are other call pending
                actionOnQuestion(PendQues);
            } else {
                // no more question
                // action on no question
                actionWithoutQuestion();
            }
        } else {
            // not admitted, call is not amonte
            add_Action(playerIx, MANO_SAYBUIDA);
            actionOnQuestion(PendQues);
        }
    } else {
        // not admitted
        add_Action(playerIx, MANO_SAYBUIDA);
        actionWithoutQuestion();
    }
}

void Mano::handleVaBene(Uint8 playerIx) {
    PendingQuestion PendQues;
    if (get_LastPendQuest(PendQues)) {
        // question was pending
        if (PendQues.isAMonte) {
            // question was a monte, accept it
            giocata_Go_Amonte(playerIx);

        } else {
            // change score question, set the new score
            _p_Score->ChangeCurrentScore(PendQues.eScore, PendQues.playerIx);
            // save the player index that make a change
            _playerChangeScore = PendQues.playerIx;
            add_Action(PendQues.eScore, MANO_CHANGESCORE);
            // clear question list
            clearQuestions();

            // restore the state before question
            restore_StateBeforeQuest();
        }

    } else {
        // no question was pending
        add_Action(playerIx, MANO_SAYBUIDA);
        actionWithoutQuestion();
    }
}

void Mano::handleVadoVia(Uint8 playerIx) {
    PendingQuestion PendQues;
    if (get_LastPendQuest(PendQues)) {
        if (PendQues.playerIx != playerIx && !PendQues.isAMonte) {
            // last change score question not accepted
            add_Action(playerIx, MANO_VADOVIA);
            // clear question list
            clearQuestions();
            _eManoState = MNST_WAIT_NEW_MANO;

        } else {
            // amonte question or the player was already asking
            add_Action(playerIx, MANO_SAYBUIDA);
            // restore trigger and state
            actionOnQuestion(PendQues);
        }
    } else {
        // no question was pending, the player leave the giocata
        add_Action(playerIx, MANO_VADOVIA);
        // clear question list
        clearQuestions();
        _eManoState = MNST_WAIT_NEW_MANO;
    }
}

void Mano::actionOnQuestion(PendingQuestion& PendQues) {
    if (IsPlayerOnCardPl()) {
        // save the state because we are going in calling phase
        save_StateBeforeQuest();
    }

    // prepare state queue on table players
    int aTablePlayer[MAX_NUM_PLAYER];
    // calculate the array of player indexes
    calcCircleIndex(aTablePlayer, MAX_NUM_PLAYER, PendQues.playerIx);
    // use the index of the next player
    int iPlayerNext = aTablePlayer[1];
    add_Action(iPlayerNext, MANO_WAIPL_TORESP);

    _eManoState = _MapManoStateResp[iPlayerNext];
}

void Mano::calcCircleIndex(int* paPlayerDeck, int size, int iPlayerIni) {
    SDL_assert(paPlayerDeck);
    SDL_assert(size > 0);

    paPlayerDeck[0] = iPlayerIni;
    int k = 1;
    while (k < _numOfPlayers && k < size) {
        paPlayerDeck[k] = paPlayerDeck[k - 1] + 1;
        if (paPlayerDeck[k] >= _numOfPlayers) {
            paPlayerDeck[k] = 0;
        }
        k++;
    }
}

void Mano::actionWithoutQuestion() {
    SDL_assert(_deqPendingQuestion.size() == 0);
    if (!IsPlayerOnCardPl()) {
        // status is not card playing, but responding
        // restore the old state playing
        restore_StateBeforeQuest();

    } else {
        // retrigger the play status
        Uint8 playerIx = _MapPlayerOnState[_eManoState];
        eFN_MANOACTION eAct_Type = _MapActionOnState[_eManoState];
        add_Action(playerIx, eAct_Type);
    }
}

void Mano::Reset() {
    _deqNextAction.clear();
    _eManoState = MNST_WAIT_NEW_MANO;
}

void Mano::add_QuestMonte(Uint8 playerIx) {
    // add the MONTE question to the queue
    PendingQuestion PendQues(true, SC_AMONTE, playerIx);
    _deqPendingQuestion.push_back(PendQues);

    actionOnQuestion(PendQues);
}

void Mano::add_Action(int iPar_0, eFN_MANOACTION eAct) {
    ActionItem Action;

    Action._eNextAction = eAct;
    Action._vct_iArg.push_back(iPar_0);
    if (eAct == MANO_WAITPL_TOPLAY || eAct == MANO_WAIPL_TORESP) {
        // remove action scheduled but not yet executed because obsolete
        removeObsoleteActions();
    }
    _deqNextAction.push_back(Action);

    STRING strActName = _MapActionNames[eAct];
    TRACE_DEBUG("Mano-> Action scheduled %s, param: %d\n", strActName.c_str(),
                iPar_0);
}

void Mano::removeObsoleteActions() {
    size_t iNumAct = _deqNextAction.size();
    for (int i = 0; i < iNumAct; i++) {
        ActionItem Action = _deqNextAction[i];
        if (Action._eNextAction == MANO_WAITPL_TOPLAY ||
            Action._eNextAction == MANO_WAIPL_TORESP) {
            // this is an obselete action, make it inoffensive
            _deqNextAction[i]._eNextAction = MANO_NO_ACTION;
        }
    }
}

bool Mano::get_LastPendQuest(PendingQuestion& PendQues) {
    bool bRes = false;
    size_t iNumEle = _deqPendingQuestion.size();
    if (iNumEle > 0) {
        PendQues = _deqPendingQuestion[iNumEle - 1];
        bRes = true;
    }
    return bRes;
}

bool Mano::get_LastPendScoreQuest(PendingQuestion& PendQues) {
    bool bRes = false;
    size_t iNumEle = _deqPendingQuestion.size();
    int iCurr = 0;
    while (iNumEle - iCurr > 0) {
        size_t iIndexLast = iNumEle - iCurr - 1;
        PendQues = _deqPendingQuestion[iIndexLast];

        if (!PendQues.isAMonte) {
            // this is a score question, ok we have it
            bRes = true;
            break;
        }
        iCurr++;
    }
    return bRes;
}

void Mano::remove_LastQuestion() {
    size_t iNumEle = _deqPendingQuestion.size();
    if (iNumEle > 0) {
        _deqPendingQuestion.pop_back();
    }
}

void Mano::restore_StateBeforeQuest() {
    _eManoState = _eOldManoState;
    eFN_MANOACTION eAct_Type = _MapActionOnState[_eManoState];

    Uint8 playerIx = _MapPlayerOnState[_eManoState];
    add_Action(playerIx, eAct_Type);
}

void Mano::save_StateBeforeQuest() { _eOldManoState = _eManoState; }

void Mano::clearQuestions() {
    TRACE_DEBUG("Clear all pending questions\n");
    _deqPendingQuestion.clear();
}

bool Mano::IsPlayerOnCardPl() {
    bool bRes = false;
    if (_eManoState == MNST_WAIT_PLAY_PLAYER_1 ||
        _eManoState == MNST_WAIT_PLAY_PLAYER_2 ||
        _eManoState == MNST_WAIT_PLAY_PLAYER_3 ||
        _eManoState == MNST_WAIT_PLAY_PLAYER_4) {
        bRes = true;
    }
    return bRes;
}

eManoStatus Mano::nextTableState() {
    SDL_assert(_deqTableState.size() > 0);
    eManoStatus eRetState = MNST_WAIT_NEW_MANO;
    if (_deqTableState.size() > 0) {
        eRetState = _deqTableState[0];
        _deqTableState.pop_front();
    }

    return eRetState;
}

void Mano::CommandWithPendingQuestion(PendingQuestion& PendQues,
                                      VCT_COMMANDS& vct_Commands,
                                      Uint8 playerIx) {
    if (playerIx == PendQues.playerIx) {
        // if pending question is from the same player,
        // he can say anything
        vct_Commands.clear();
        return;
    }
    // the question is an opponent question
    if (PendQues.isAMonte) {
        // pending question is "a monte"
        vct_Commands.push_back(eSayPlayer::SP_VABENE);
        vct_Commands.push_back(eSayPlayer::SP_NO);
    } else {
        // pending question is score
        vct_Commands.push_back(eSayPlayer::SP_VADOVIA);
        vct_Commands.push_back(eSayPlayer::SP_AMONTE);
        vct_Commands.push_back(eSayPlayer::SP_GIOCA);
        // get the next score
        if (PendQues.eScore != eGiocataScoreState::SC_PARTIDA) {
            // it is possible to call more score
            eGiocataScoreState eNextScore = _mapScoreScNext[PendQues.eScore];
            eSayPlayer eCallMore = _mapScoreSay[eNextScore];
            vct_Commands.push_back(eCallMore);
        }
    }
}

void Mano::GetMoreCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIx) {
    vct_Commands.clear();

    PendingQuestion PendQues;
    if (get_LastPendQuest(PendQues)) {
        CommandWithPendingQuestion(PendQues, vct_Commands, playerIx);
        return;
    }
    vct_Commands.push_back(eSayPlayer::SP_VADODENTRO);
    if (_playerChangeScore == playerIx) {
        vct_Commands.push_back(eSayPlayer::SP_CHIAMADIPIU);
    } else if (_playerChangeScore == eGameConst::NOT_VALID_INDEX) {
        vct_Commands.push_back(eSayPlayer::SP_CHIAMA_BORTOLO);
    }
}

void Mano::GetAdmittedCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIx) {
    vct_Commands.clear();

    PendingQuestion PendQues;
    eSayPlayer eSayAvail;
    if (get_LastPendQuest(PendQues)) {
        CommandWithPendingQuestion(PendQues, vct_Commands, playerIx);
        return;
    }

    // no pending questions

    // check if the giocata is a monte
    if (isGiocataAMonte()) {
        // game is gone a monte, no command available
        vct_Commands.clear();
    } else {
        // we are on game
        vct_Commands.push_back(eSayPlayer::SP_VADOVIA);
        vct_Commands.push_back(eSayPlayer::SP_AMONTE);
        if (_playerChangeScore != playerIx) {
            // player can call in order to increase the score
            if (nextAvailSayScore(&eSayAvail)) {
                vct_Commands.push_back(eSayAvail);
            }
        } else {
            // increment score not admitted
        }
    }
}

bool Mano::nextAvailSayScore(eSayPlayer* peSayAvail) {
    bool bRet = true;
    SDL_assert(peSayAvail);
    eSayPlayer eTmp = eSayPlayer::SP_AMONTE;

    // retrive current score
    MatchPoints* pMatchPoints = _p_InvidoCore->GetMatchPointsInstance();
    SDL_assert(pMatchPoints);
    eGiocataScoreState eCurrScore = pMatchPoints->GetCurrScore();

    if (eCurrScore == SC_PARTIDA) {
        // there is no points bigger as "partida"
        bRet = false;
    } else {
        switch (eCurrScore) {
            case SC_CANELA:
                eTmp = eSayPlayer::SP_INVIDO;
                break;
            case SC_INVIDO:
                eTmp = eSayPlayer::SP_TRASMAS;
                break;

            case SC_TRASMAS:
                eTmp = eSayPlayer::SP_TRASMASNOEF;
                break;

            case SC_TRASMASNOEF:
                eTmp = eSayPlayer::SP_FUERAJEUQ;
                break;

            case SC_FUERAJEUQ:
                eTmp = eSayPlayer::SP_PARTIDA;
                break;

            default:
                SDL_assert(0);
                break;
        }
    }
    *peSayAvail = eTmp;
    return bRet;
}

bool Mano::isGiocataAMonte() {
    bool bRet = false;
    MatchPoints* pMatchPoints = _p_InvidoCore->GetMatchPointsInstance();
    SDL_assert(pMatchPoints);
    eGiocataScoreState eCurrScore = pMatchPoints->GetCurrScore();
    if (eCurrScore == eGiocataScoreState::SC_AMONTE ||
        eCurrScore == eGiocataScoreState::SC_PATTA) {
        bRet = true;
    }
    return bRet;
}
