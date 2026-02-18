#include "Partita.h"

#include "Giocata.h"
#include "InvidoCore.h"
#include "InvidoCoreEnv.h"
#include "MatchPoints.h"

using namespace invido;

Partita::Partita() {
    m_pGiocata = NULL;
    m_pInvidoCore = NULL;
    m_PartitaState = WAIT_NEW_PARTITA;
    m_lGiocStart = 0;
    _eNextAction = NO_ACTION;
}

LPErrInApp Partita::NewPartita(long lPlayerIx) {
    m_lGiocStart = lPlayerIx;

    if (m_PartitaState != PARTITA_ONGOING) {
        m_PartitaState = PARTITA_ONGOING;

        m_pGiocata->NewGiocata(m_lGiocStart);
    } else {
        return ERR_UTIL::ErrorCreate("Partita state is not correct %d",
                                     m_PartitaState);
    }
    return NULL;
}

void Partita::NextAction() {
    switch (_eNextAction) {
        case ACT_PARTITA_END:
            // partita is eneded
            m_pInvidoCore->Partita_End();
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
    SDL_assert(m_PartitaState == PARTITA_ONGOING);

    if (pIScore->IsMatchEnd()) {
        // match is ended
        _eNextAction = ACT_PARTITA_END;
        m_PartitaState = PARTITA_END;
    } else {
        // start a new giocata
        switch (m_lGiocStart) {
            case 0:
                m_lGiocStart = 1;
                break;
            case 1:
                m_lGiocStart = 0;
                break;
            default:
                return ERR_UTIL::ErrorCreate(
                    "[Update_Partita] Index out of bound");
        }
        m_pGiocata->NewGiocata(m_lGiocStart);
    }
    return NULL;
}

void Partita::Reset() {
    m_PartitaState = WAIT_NEW_PARTITA;
    m_lGiocStart = 0;
}
