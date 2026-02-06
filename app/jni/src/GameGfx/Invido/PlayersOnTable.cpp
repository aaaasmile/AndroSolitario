

// cPlayersOnTable

#include "cPlayersOnTable.h"

/////////////////////////////////////////////////////////////////////////////////////
// *********************************     CPLAYERSONTABLE CLASS
// *********************
/////////////////////////////////////////////////////////////////////////////////////

//! constructor
cPlayersOnTable::cPlayersOnTable() {
    m_lCurrent = 0;
    m_lNumPlayers = 0;
    m_lFirstOnTrick = NOT_VALID_INDEX;
    m_lFirstOnGiocata = NOT_VALID_INDEX;
    m_lFirstOnMatch = NOT_VALID_INDEX;
}

void cPlayersOnTable::SetFirstOnTrick(long lIndex) {
    if (lIndex < (long)m_vctPlayers.size() && lIndex >= 0) {
        m_lCurrent = lIndex;
        m_lFirstOnTrick = lIndex;
    } else {
        ASSERT(0);
    }
}

void cPlayersOnTable::SetFirstOnGiocata(long lIndex) {
    if (lIndex < (long)m_vctPlayers.size() && lIndex >= 0) {
        m_lCurrent = lIndex;
        m_lFirstOnTrick = lIndex;
        m_lFirstOnGiocata = lIndex;
    } else {
        ASSERT(0);
    }
}

void cPlayersOnTable::SetFirstOnMatch(long lIndex) {
    if (lIndex < (long)m_vctPlayers.size() && lIndex >= 0) {
        m_lCurrent = lIndex;
        m_lFirstOnTrick = lIndex;
        m_lFirstOnGiocata = lIndex;
        m_lFirstOnMatch = lIndex;
    } else {
        ASSERT(0);
    }
}

void cPlayersOnTable::Create(cPlayer* pHmiPlayer, int iNumPlayers) {
    m_vctPlayers.clear();
    for (int i = 0; i < iNumPlayers; i++) {
        if (pHmiPlayer && i == 0) {
            pHmiPlayer->SetIndex(0);
            m_vctPlayers.push_back(*pHmiPlayer);
        } else {
            m_vctPlayers.push_back(cPlayer());
            m_vctPlayers[i].Create();
            // type is default value. Gfx engine change it.
            if (i == 0) {
                // the first player is a local
                m_vctPlayers[i].SetType(PT_LOCAL);
            } else {
                // all others are machine
                m_vctPlayers[i].SetType(PT_MACHINE);
            }
            m_vctPlayers[i].SetIndex(i);
        }
    }

    m_lNumPlayers = iNumPlayers;
}

cPlayer* cPlayersOnTable::GetPlayerToPlay(eSwitchPLayer eVal) {
    size_t lNumPlayers = m_vctPlayers.size();
    long lTemp = m_lCurrent;

    if (eVal == SWITCH_TO_NEXT) {
        // current is the next
        m_lCurrent++;

        if (m_lCurrent >= lNumPlayers) {
            m_lCurrent = 0;
        }
    }

    return &m_vctPlayers[lTemp];
}

cPlayer* cPlayersOnTable::GetPlayerIndex(long lIndex) {
    if (lIndex < (long)m_vctPlayers.size() && lIndex >= 0) {
    } else {
        ASSERT(0);
    }

    return &m_vctPlayers[lIndex];
}

int cPlayersOnTable::CalcDistance(int iPlayerRef, int iPlayerTmp) {
    int aTableIx[MAX_NUM_PLAYER];
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        aTableIx[i] = i;
    }
    int iTmp = iPlayerRef;
    int iDist = 0;

    ASSERT(iPlayerTmp >= 0 && iPlayerTmp < m_lNumPlayers);
    ASSERT(iPlayerRef >= 0 && iPlayerRef < m_lNumPlayers);

    BOOL bFound = FALSE;
    while (!bFound && iDist < m_lNumPlayers) {
        if (aTableIx[iTmp] == iPlayerTmp) {
            bFound = TRUE;
        } else {
            iDist++;
            iTmp++;
            if (iTmp >= m_lNumPlayers) {
                iTmp = 0;
            }
        }
    }
    ASSERT(bFound);
    return iDist;
}

void cPlayersOnTable::CalcCircleIndex(int* paPlayerDeck) {
    ASSERT(paPlayerDeck);
    paPlayerDeck[0] = m_lCurrent;
    int k = 1;
    while (k < m_lNumPlayers) {
        paPlayerDeck[k] = paPlayerDeck[k - 1] + 1;
        if (paPlayerDeck[k] >= m_lNumPlayers) {
            paPlayerDeck[k] = 0;
        }
        k++;
    }
}

BOOL cPlayersOnTable::IsLevelPython() {
    BOOL bRes = FALSE;

    for (int i = 0; i < m_lNumPlayers; i++) {
        eGameLevel eLevel = m_vctPlayers[i].GetLevel();
        if (eLevel == TEST_PYTHON) {
            bRes = TRUE;
            break;
        }
    }

    return bRes;
}

void cPlayersOnTable::CalcCircleIndex_Cust(int* paPlayerDeck, int iPlayerIni) {
    ASSERT(paPlayerDeck);

    paPlayerDeck[0] = iPlayerIni;
    int k = 1;
    while (k < m_lNumPlayers) {
        paPlayerDeck[k] = paPlayerDeck[k - 1] + 1;
        if (paPlayerDeck[k] >= m_lNumPlayers) {
            paPlayerDeck[k] = 0;
        }
        k++;
    }
}