#include "PlayersOnTable.h"

using namespace invido;

PlayersOnTable::PlayersOnTable() {
    _current = 0;
    _numPlayers = 0;
    _firstOnTrick = 0;
    _firstOnGiocata = 0;
    _firstOnMatch = 0;
}

void PlayersOnTable::SetFirstOnTrick(Uint8 index) {
    if (index < _vctPlayers.size() && index >= 0) {
        _current = index;
        _firstOnTrick = index;
    } else {
        SDL_assert(0);
    }
}

void PlayersOnTable::SetFirstOnGiocata(Uint8 index) {
    if (index < _vctPlayers.size() && index >= 0) {
        _current = index;
        _firstOnTrick = index;
        _firstOnGiocata = index;
    } else {
        SDL_assert(0);
    }
}

void PlayersOnTable::SetFirstOnMatch(Uint8 index) {
    if (index < _vctPlayers.size() && index >= 0) {
        _current = index;
        _firstOnTrick = index;
        _firstOnGiocata = index;
        _firstOnMatch = index;
    } else {
        SDL_assert(0);
    }
}

void PlayersOnTable::Create(Player* pHmiPlayer, int iNumPlayers) {
    _vctPlayers.clear();
    for (int i = 0; i < iNumPlayers; i++) {
        if (pHmiPlayer && i == 0) {
            pHmiPlayer->SetIndex(0);
            _vctPlayers.push_back(*pHmiPlayer);
        } else {
            _vctPlayers.push_back(Player());
            _vctPlayers[i].Create();
            // type is default value. Gfx engine change it.
            if (i == 0) {
                // the first player is a local
                _vctPlayers[i].SetType(PT_LOCAL);
            } else {
                // all others are machine
                _vctPlayers[i].SetType(PT_MACHINE);
            }
            _vctPlayers[i].SetIndex(i);
        }
    }

    _numPlayers = iNumPlayers;
}

Player* PlayersOnTable::GetPlayerToPlay(eSwitchPLayer eVal) {
    size_t numPlayers = _vctPlayers.size();
    Uint8 playerIx = _current;

    if (eVal == SWITCH_TO_NEXT) {
        // current is the next
        _current++;

        if (_current >= numPlayers) {
            _current = 0;
        }
    }

    return &_vctPlayers[playerIx];
}

Player* PlayersOnTable::GetPlayerIndex(Uint8 index) {
    if (index < _vctPlayers.size() && index >= 0) {
    } else {
        SDL_assert(0);
    }

    return &_vctPlayers[index];
}

int PlayersOnTable::CalcDistance(int iPlayerRef, int iPlayerTmp) {
    int aTableIx[MAX_NUM_PLAYER];
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        aTableIx[i] = i;
    }
    int iTmp = iPlayerRef;
    int iDist = 0;

    SDL_assert(iPlayerTmp >= 0 && iPlayerTmp < _numPlayers);
    SDL_assert(iPlayerRef >= 0 && iPlayerRef < _numPlayers);

    bool bFound = false;
    while (!bFound && iDist < _numPlayers) {
        if (aTableIx[iTmp] == iPlayerTmp) {
            bFound = true;
        } else {
            iDist++;
            iTmp++;
            if (iTmp >= _numPlayers) {
                iTmp = 0;
            }
        }
    }
    SDL_assert(bFound);
    return iDist;
}

void PlayersOnTable::CalcCircleIndex(int* paPlayerDeck) {
    SDL_assert(paPlayerDeck);
    paPlayerDeck[0] = _current;
    int k = 1;
    while (k < _numPlayers) {
        paPlayerDeck[k] = paPlayerDeck[k - 1] + 1;
        if (paPlayerDeck[k] >= _numPlayers) {
            paPlayerDeck[k] = 0;
        }
        k++;
    }
}

bool PlayersOnTable::IsLevelPython() {
    bool bRes = false;

    for (int i = 0; i < _numPlayers; i++) {
        eGameLevel eLevel = _vctPlayers[i].GetLevel();
        if (eLevel == TEST_PYTHON) {
            bRes = true;
            break;
        }
    }

    return bRes;
}

void PlayersOnTable::CalcCircleIndex_Cust(int* paPlayerDeck, int iPlayerIni) {
    SDL_assert(paPlayerDeck);

    paPlayerDeck[0] = iPlayerIni;
    int k = 1;
    while (k < _numPlayers) {
        paPlayerDeck[k] = paPlayerDeck[k - 1] + 1;
        if (paPlayerDeck[k] >= _numPlayers) {
            paPlayerDeck[k] = 0;
        }
        k++;
    }
}