#include "Giocata.h"

#include "InvidoCore.h"
#include "Mano.h"
#include "MatchPoints.h"

using namespace invido;

Giocata::Giocata() {
    _p_InvidoCore = NULL;
    _p_Partita = NULL;
    _p_Mano = NULL;
    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

LPErrInApp Giocata::NewGiocata(Uint8 playerIx) {
    ActionItemGio Action;

    if (_eGiocataStatus == WAIT_NEW_GIOCATA) {
        _eGiocataStatus = GIOCATA_ONGOING;
        // next action
        Action.m_vct_lArg.push_back(playerIx);
        Action._eNextAction = GIOC_START;
        _deqNextAction.push_back(Action);
    } else {
        return ERR_UTIL::ErrorCreate("Giocata state is not correct %d",
                                     _eGiocataStatus);
    }
    return NULL;
}

void Giocata::NextAction() {
    size_t iNumAct = _deqNextAction.size();
    if (iNumAct == 0) {
        // no action
        return;
    }
    ActionItemGio Action = _deqNextAction.front();
    _deqNextAction.pop_front();

    switch (Action._eNextAction) {
        case GIOC_START:
            // gioca is started
            SDL_assert(Action.m_vct_lArg.size() > 0);
            _p_InvidoCore->Giocata_Start(Action.m_vct_lArg[0]);
            // mano state
            _p_Mano->NewMano(Action.m_vct_lArg[0]);

            break;

        case GIOC_END:
            // giocata is eneded
            _p_InvidoCore->Giocata_End();
            break;

        case GIOC_NO_ACTION:
            // nothing to do
            break;

        default:
            SDL_assert(0);
            break;
    }
}

void Giocata::Update_Giocata(Uint8 playerIx, I_MatchScore* pIScore) {
    // check mano
    SDL_assert(_eGiocataStatus == GIOCATA_ONGOING);
    SDL_assert(pIScore);
    ActionItemGio Action;

    if (pIScore->IsGiocatEnd()) {
        // giocata is terminated
        _eGiocataStatus = WAIT_NEW_GIOCATA;
        Action._eNextAction = GIOC_END;
        _deqNextAction.push_back(Action);
    } else {
        _p_Mano->NewMano(playerIx);
    }
}

void Giocata::Reset() {
    _eGiocataStatus = WAIT_NEW_GIOCATA;
    _deqNextAction.clear();
}
