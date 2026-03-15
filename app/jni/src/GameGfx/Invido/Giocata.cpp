#include "Giocata.h"

#include "InvidoCore.h"
#include "Mano.h"
#include "MatchPoints.h"

using namespace invido;

Giocata::Giocata() { _eGiocataStatus = WAIT_NEW_GIOCATA; }

void Giocata::NextAction() {
    size_t numAct = _deqNextAction.size();
    if (numAct == 0) {
        // no action
        return;
    }
    ActionItemGio action = _deqNextAction.front();
    _deqNextAction.pop_front();

    switch (action._eNextAction) {
        case GIOC_STARTED:
            _giocataCb.tc->GiocataStarted(_giocataCb.self, action._playerIx);
            break;

        case GIOC_WITHWIN_ENDED:
            _giocataCb.tc->GiocataEndedWinner(_giocataCb.self,
                                              action._playerIx);
            break;

        case GIOC_NOWINNERS_ENDED:
            _giocataCb.tc->GiocataEndDueced(_giocataCb.self);
            break;

        default:
            SDL_assert(0);
            break;
    }
}

void Giocata::Reset() {
    _eGiocataStatus = WAIT_NEW_GIOCATA;
    _deqNextAction.clear();
}

void Giocata::PlayerGiocataWins(Uint8 playerWinnerIx) {
    SDL_assert(_eGiocataStatus == GIOCATA_ONGOING);

    _eIsGiocataEnd = GES_HAVE_WINNER;
    ActionItemGio action = {._eNextAction = GIOC_WITHWIN_ENDED,
                            ._playerIx = playerWinnerIx};
    _deqNextAction.push_back(action);

    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

void Giocata::GiocataAMonte() {
    _eIsGiocataEnd = GES_AMONTE;
    ActionItemGio action = {._eNextAction = GIOC_NOWINNERS_ENDED};
    _deqNextAction.push_back(action);
    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

void Giocata::GiocataPatada() {
    _eIsGiocataEnd = GES_PATADA;
    ActionItemGio action = {._eNextAction = GIOC_NOWINNERS_ENDED};
    _deqNextAction.push_back(action);
    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

LPErrInApp Giocata::NewGiocata(Uint8 playerIx) {
    if (_eGiocataStatus == WAIT_NEW_GIOCATA) {
        _eGiocataStatus = GIOCATA_ONGOING;
        ActionItemGio action = {._eNextAction = GIOC_STARTED,
                                ._playerIx = playerIx};
        _deqNextAction.push_back(action);
    } else {
        return ERR_UTIL::ErrorCreate("Giocata state is not correct %d",
                                     _eGiocataStatus);
    }
    return NULL;
}
