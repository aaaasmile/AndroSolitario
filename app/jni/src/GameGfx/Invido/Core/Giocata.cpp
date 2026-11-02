#include "Giocata.h"

#include "InvidoCore.h"
#include "Mano.h"
#include "MatchStateWithScore.h"

using namespace invido;

Giocata::Giocata() {
    _eGiocataStatus = WAIT_NEW_GIOCATA;
    _numGiocata = 0;
}

LPErrInApp Giocata::NextAction(bool& hasAction) {
    hasAction = false;
    if (_deqNextAction.empty()) {
        return NULL;
    }
    hasAction = true;

    ActionItemGio action = _deqNextAction.front();
    _deqNextAction.pop_front();

    const char* actionName = "Unknown";
    switch (action._eNextAction) {
        case GIOC_STARTED:
            actionName = "Started";
            break;
        case GIOC_WITHWIN_ENDED:
            actionName = "EndedWithWinner";
            break;
        case GIOC_NOWINNERS_ENDED:
            actionName = "EndedWithoutWinner";
            break;
        default:
            SDL_assert(0);
            break;
    }
    _giocataCb.tc->ActionExecuted(_giocataCb.self, actionName,
                                  action._playerIx);

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
    return NULL;
}

void Giocata::Reset() {
    _eGiocataStatus = WAIT_NEW_GIOCATA;
    _deqNextAction.clear();
    _numGiocata = 0;
}

void Giocata::PlayerGiocataWins(Uint8 playerWinnerIx) {
    TRACE_DEBUG("[Giocata] PlayerGiocataWins %d\n", playerWinnerIx);
    SDL_assert(_eGiocataStatus == GIOCATA_ONGOING);

    ActionItemGio action = {._eNextAction = GIOC_WITHWIN_ENDED,
                            ._playerIx = playerWinnerIx};
    _deqNextAction.push_back(action);

    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

void Giocata::GiocataAMonte() {
    TRACE_DEBUG("[Giocata] GiocataAMonte \n");
    ActionItemGio action = {._eNextAction = GIOC_NOWINNERS_ENDED};
    _deqNextAction.push_back(action);
    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

void Giocata::GiocataPatada() {
    TRACE_DEBUG("[Giocata] GiocataPatada \n");
    ActionItemGio action = {._eNextAction = GIOC_NOWINNERS_ENDED};
    _deqNextAction.push_back(action);
    _eGiocataStatus = WAIT_NEW_GIOCATA;
}

LPErrInApp Giocata::NewGiocata(Uint8 playerIx) {
    _numGiocata++;
    TRACE_DEBUG("[Giocata] NewGiocata(%d), player start: %d \n", _numGiocata,
                playerIx);
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
