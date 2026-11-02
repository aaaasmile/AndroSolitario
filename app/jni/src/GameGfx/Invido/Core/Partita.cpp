#include "Partita.h"

#include "Giocata.h"
#include "InvidoCore.h"
#include "InvidoCoreDef.h"
#include "MatchStateWithScore.h"

using namespace invido;

Partita::Partita() { _PartitaState = ePartitaStatus::WAIT_NEW_PARTITA; }

LPErrInApp Partita::NextAction(bool& hasAction) {
    hasAction = false;
    if (_deqNextAction.empty()) {
        return NULL;
    }
    hasAction = true;

    ActionItemPartita action = _deqNextAction.front();
    _deqNextAction.pop_front();

    const char* actionName = "Unknown";
    switch (action._eNextAction) {
        case ACT_PARTITA_STARTED:
            actionName = "Started";
            break;
        case ACT_PARTITA_END:
            actionName = "Ended";
            break;
        default:
            SDL_assert(0);
            break;
    }
    const int playerIndex = action._eNextAction == ACT_PARTITA_STARTED
                                ? action._playerIx
                                : NOT_VALID_INDEX;
    _partitaCb.tc->ActionExecuted(_partitaCb.self, actionName, playerIndex);

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
    return NULL;
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
