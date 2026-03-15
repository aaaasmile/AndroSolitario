#include "Partita.h"

#include "Giocata.h"
#include "InvidoCore.h"
#include "InvidoCoreDef.h"
#include "MatchPoints.h"

using namespace invido;

Partita::Partita() {
    _pGiocata = NULL;
    _pInvidoCore = NULL;
    _PartitaState = WAIT_NEW_PARTITA;
    _playerStartIx = 0;
    _eNextAction = NO_ACTION;
}

LPErrInApp Partita::NewPartita(Uint8 playerStartIx) {
    _playerStartIx = playerStartIx;

    if (_PartitaState != PARTITA_ONGOING) {
        _PartitaState = PARTITA_ONGOING;

        _pGiocata->NewGiocata(_playerStartIx);
    } else {
        return ERR_UTIL::ErrorCreate("Partita state is not correct %d",
                                     _PartitaState);
    }
    return NULL;
}

void Partita::NextAction() {
    switch (_eNextAction) {
        case ACT_PARTITA_END:
            // partita is ended
            _pInvidoCore->Partita_End();
            break;

        case NO_ACTION:
            // nothing to do
            break;

        default:
            SDL_assert(0);
            break;
    }

    _eNextAction = NO_ACTION;
}

LPErrInApp Partita::Update_Partita(I_MatchScore* pIScore) {
    SDL_assert(pIScore);
    SDL_assert(_PartitaState == PARTITA_ONGOING);

    if (pIScore->IsMatchEnd()) {
        // match is ended
        _eNextAction = ACT_PARTITA_END;
        _PartitaState = PARTITA_END;
    } else {
        // start a new giocata
        switch (_playerStartIx) {
            case 0:
                _playerStartIx = 1;
                break;
            case 1:
                _playerStartIx = 0;
                break;
            default:
                return ERR_UTIL::ErrorCreate(
                    "[Update_Partita] Index out of bound");
        }
        _pGiocata->NewGiocata(_playerStartIx);
    }
    return NULL;
}

void Partita::Reset() {
    _PartitaState = WAIT_NEW_PARTITA;
    _playerStartIx = 0;
}
