#include "PlayersOnTable.h"

using namespace invido;

PlayersOnTable::PlayersOnTable() {
    _current = 0;
    _firstOnTrick = 0;
    _firstOnGiocata = 0;
    _firstOnMatch = 0;
}

void PlayersOnTable::SetCurrentAndFirstOnTrick(Uint8 index) {
    if (index < _vctPlayers.size() && index >= 0) {
        _current = index;
        _firstOnTrick = index;
    } else {
        SDL_assert(0);
    }
}

Player* PlayersOnTable::PeekNextPlayerToIx(Uint8 index) {
    Uint8 next = _current + 1;
    if (next >= _vctPlayers.size()) {
        next = 0;
    }
    return &_vctPlayers[next];
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

void PlayersOnTable::AddPlayer(Player& player) {
    _vctPlayers.push_back(player);
}

Player* PlayersOnTable::GetPlayerToPlay() {
    size_t numPlayers = _vctPlayers.size();
    Uint8 playerIx = _current;
    SDL_assert(playerIx < numPlayers && playerIx >= 0);
    return &_vctPlayers[playerIx];
}

Player* PlayersOnTable::SwitchToNextPlayer() {
    size_t numPlayers = _vctPlayers.size();
    Uint8 playerIx = _current;
    _current++;
    if (_current >= numPlayers) {
        _current = 0;
    }
    SDL_assert(playerIx < _vctPlayers.size() && playerIx >= 0);
    return &_vctPlayers[playerIx];
}

Player* PlayersOnTable::GetPlayerOnIndex(Uint8 index) {
    SDL_assert(index < _vctPlayers.size() && index >= 0);
    return &_vctPlayers[index];
}

int PlayersOnTable::CalcDistance(int playerIxFrom, int playerIxTo) {
    int iDist = 0;
    size_t numPlayers = _vctPlayers.size();

    SDL_assert(playerIxTo >= 0 && playerIxTo < numPlayers);
    SDL_assert(playerIxFrom >= 0 && playerIxFrom < numPlayers);

    bool bFound = false;
    while (!bFound && iDist < numPlayers) {
        if (playerIxFrom == playerIxTo) {
            bFound = true;
        } else {
            iDist++;
            playerIxFrom++;
            if (playerIxFrom >= numPlayers) {
                playerIxFrom = 0;
            }
        }
    }
    SDL_assert(bFound);
    return iDist;
}

void PlayersOnTable::CalcCircleIndex(int* paPlayerDeck, size_t size) {
    SDL_assert(paPlayerDeck);
    Uint8 iniVal = _current;
    paPlayerDeck[0] = iniVal;
    int k = 1;
    size_t numPlayers = _vctPlayers.size();
    SDL_assert(numPlayers <= size);
    while (k < numPlayers) {
        iniVal++;
        if (iniVal >= numPlayers) {
            iniVal = 0;
        }
        paPlayerDeck[k] = iniVal;
        k++;
    }
}
