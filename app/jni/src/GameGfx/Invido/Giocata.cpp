

// Giocata.cpp

#include "Giocata.h"

#include "InvidoCore.h"
#include "Mano.h"
#include "MatchPoints.h"

/////////////////////////////////////////////////////////////////////////////////////
//   *******************  CGIOCATA CLASS ***************************************
/////////////////////////////////////////////////////////////////////////////////////
Giocata::Giocata() {
    m_pInvidoCore = NULL;
    m_pPartita = NULL;
    m_pMano = NULL;
    m_eGiocataStatus = WAIT_NEW_GIOCATA;
}

void Giocata::NewGiocata(long lPlayerIx) {
    ActionItemGio Action;

    if (m_eGiocataStatus == WAIT_NEW_GIOCATA) {
        m_eGiocataStatus = GIOCATA_ONGOING;
        // next action
        Action.m_vct_lArg.push_back(lPlayerIx);
        Action.m_eNextAction = GIOC_START;
        m_deqNextAction.push_back(Action);
    } else {
        m_pInvidoCore->RaiseError("Giocata state not right\n");
        SDL_assert(0);
    }
}

void Giocata::NextAction() {
    size_t iNumAct = m_deqNextAction.size();
    if (iNumAct == 0) {
        // no action
        return;
    }
    ActionItemGio Action = m_deqNextAction.front();
    m_deqNextAction.pop_front();

    switch (Action.m_eNextAction) {
        case GIOC_START:
            // gioca is started
            SDL_assert(Action.m_vct_lArg.size() > 0);
            m_pInvidoCore->Giocata_Start(Action.m_vct_lArg[0]);
            // mano state
            m_pMano->NewMano(Action.m_vct_lArg[0]);

            break;

        case GIOC_END:
            // giocata is eneded
            m_pInvidoCore->Giocata_End();
            break;

        case GIOC_NO_ACTION:
            // nothing to do
            break;

        default:
            SDL_assert(0);
            break;
    }
}

void Giocata::Update_Giocata(long lPlayerIx, I_MatchScore* pIScore) {
    // check mano
    SDL_assert(m_eGiocataStatus == GIOCATA_ONGOING);
    SDL_assert(pIScore);
    ActionItemGio Action;

    if (pIScore->IsGiocatEnd()) {
        // giocata is terminated
        m_eGiocataStatus = WAIT_NEW_GIOCATA;
        Action.m_eNextAction = GIOC_END;
        m_deqNextAction.push_back(Action);
    } else {
        m_pMano->NewMano(lPlayerIx);
    }
}

void Giocata::Reset() {
    m_eGiocataStatus = WAIT_NEW_GIOCATA;
    m_deqNextAction.clear();
}
