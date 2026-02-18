#include "Mazzo.h"

#include "InvidoCore.h"

using namespace invido;

Mazzo::Mazzo() {
    _nextCard = 0;
    _p_CoreGame = 0;
    _rndSeed = 63200;
}

void Mazzo::Create() {
    _vctCardIndex.reserve(NUM_CARDS);
    _vctCardIndex.clear();

    // invido card index
    int aCardIndex[] = {0,  1,  2,  5,  6,  7,  8,  9,  10, 11, 12,
                        15, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27,
                        28, 29, 30, 31, 32, 35, 36, 37, 38, 39};

    for (int i = 0; i < NUM_CARDS; i++) {
        _vctCardIndex.push_back(aCardIndex[i]);
    }

    _nextCard = 0;
}

void Mazzo::SetIndexRaw(int iIndex, long lVal) {
    if (iIndex < (int)_vctCardIndex.size() && iIndex >= 0) {
        _vctCardIndex[iIndex] = lVal;
    }
}

bool Mazzo::Shuffle() {
    IT_VCTLONG it_tmp;

    _nextCard = 0;

    it_tmp = _vctCardIndex.begin();
    // Leave the deck card to the first position

    // use SDL_rand to shuffle the pool
    for (size_t i = _vctCardIndex.size() - 1; i > 0; --i) {
        size_t j = SDL_rand((Uint32)(i + 1));
        std::swap(_vctCardIndex[i], _vctCardIndex[j]);
    }

    _p_CoreGame->NotifyScript(SCR_NFY_SHUFFLEDECK);

    return true;
}

CardSpec* Mazzo::PickNextCard(bool* pIsValid) {
    SDL_assert(pIsValid);
    *pIsValid = false;
    CardSpec* pRes = NULL;
    if (_nextCard < _vctCardIndex.size()) {
        *pIsValid = true;
        pRes = &_arrCardSpec[_nextCard];
        pRes->SetCardIndex(_vctCardIndex[_nextCard]);
        _nextCard++;
    }
    return pRes;
}
