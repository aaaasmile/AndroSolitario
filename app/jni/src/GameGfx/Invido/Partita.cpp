#include "Partita.h"

#include "Giocata.h"
#include "InvidoCore.h"
#include "InvidoCoreDef.h"
#include "MatchPoints.h"

using namespace invido;

Partita::Partita() { _PartitaState = ePartitaStatus::WAIT_NEW_PARTITA; }

void Partita::NextAction() {
    size_t numAct = _deqNextAction.size();
    if (numAct == 0) {
        return;
    }
    ActionItemPartita action = _deqNextAction.front();
    _deqNextAction.pop_front();

    switch (action._eNextAction) {
        case ACT_PARTITA_STARTED:
            _partitaCb.tc->PartitaStarted(_partitaCb.self, action._playerIx);
            break;

        case ACT_PARTITA_END:
            _partitaCb.tc->PartitaEnded(_partitaCb.self);
            break;

        default:
            SDL_assert(0);
            break;
    }
}

void Partita::Reset() { 
    _PartitaState = ePartitaStatus::WAIT_NEW_PARTITA; 
    _deqNextAction.clear();
}

LPErrInApp Partita::NewPartita(Uint8 playerStartIx) {
    if (_PartitaState != ePartitaStatus::PARTITA_ONGOING) {
        _PartitaState = ePartitaStatus::PARTITA_ONGOING;
        ActionItemPartita action = {._eNextAction = ACT_PARTITA_STARTED,
                                    ._playerIx = playerStartIx};
        _deqNextAction.push_back(action);
    } else {
        return ERR_UTIL::ErrorCreate("Partita state is not correct %d",
                                     _PartitaState);
    }
    return NULL;
}

void Partita::PartitaEnd() {
    _PartitaState = ePartitaStatus::PARTITA_END;
    ActionItemPartita action = {._eNextAction = ACT_PARTITA_END};
    _deqNextAction.push_back(action);
}
