#include "Mazzo.h"

#include "InvidoCore.h"

Mazzo::Mazzo() {
    _nextCard = 0;
    _p_CoreGame = 0;
    _rndSeed = 63200;
}

void Mazzo::Create() {
    _vctCards.reserve(NUM_CARDS);
    _vctCards.clear();

    // invido card index
    int aCardIndex[] = {0,  1,  2,  5,  6,  7,  8,  9,  10, 11, 12,
                        15, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27,
                        28, 29, 30, 31, 32, 35, 36, 37, 38, 39};

    for (int i = 0; i < NUM_CARDS; i++) {
        _vctCards.push_back(aCardIndex[i]);
    }

    _nextCard = 0;
}

void Mazzo::SetIndexRaw(int iIndex, long lVal) {
    if (iIndex < (int)_vctCards.size() && iIndex >= 0) {
        _vctCards[iIndex] = lVal;
    }
}

bool Mazzo::CloneFrom(Mazzo& Master) {
    _nextCard = Master.GetNextCardVal();
    _vctCards = Master.GetVectorIndexes();

    return true;
}

bool Mazzo::Shuffle() {
    IT_VCTLONG it_tmp;

    _nextCard = 0;

    it_tmp = _vctCards.begin();
    // Leave the deck card to the first position

    // use SDL_rand to shuffle the pool
    for (size_t i = _vctCards.size() - 1; i > 0; --i) {
        size_t j = SDL_rand((Uint32)(i + 1));
        std::swap(_vctCards[i], _vctCards[j]);
    }

    // call a callback in python script for shuffle deck
    _p_CoreGame->NotifyScript(SCR_NFY_SHUFFLEDECK);

    return true;
}

long Mazzo::PickNextCard(bool* pbEnd) {
    long lResult = NOT_VALID_INDEX;
    if (_nextCard >= (long)_vctCards.size()) {
        *pbEnd = false;
        return lResult;
    }
    *pbEnd = true;

    lResult = _vctCards[_nextCard];
    _nextCard++;

    return lResult;
}

bool Mazzo::PickNextCard(CardSpec* pRes) {
    SDL_assert(pRes);

    pRes->Reset();
    bool bValid = false;

    if (_nextCard < (long)_vctCards.size()) {
        bValid = true;
        long lIndex = _vctCards[_nextCard];

        pRes->SetCardIndex(lIndex);

        _nextCard++;
    }

    return bValid;
}

long Mazzo::ThrowTableCard() {
    long lResult = NOT_VALID_INDEX;
    if (_nextCard >= (long)_vctCards.size()) {
        return lResult;
    }

    lResult = _vctCards[_nextCard];

    // swap the next card with the last
    long lLast = _vctCards[NUM_CARDS - 1];
    _vctCards[NUM_CARDS - 1] = _vctCards[_nextCard];
    _vctCards[_nextCard] = lLast;

    return lResult;
}

void Mazzo::TraceIt() {}

bool Mazzo::IsMoreCards() {
    bool bRet = false;
    if (_nextCard < NUM_CARDS) {
        bRet = true;
    }
    return bRet;
}

long Mazzo::GetIndexNextCard(bool* pbEnd) {
    long lResult = NOT_VALID_INDEX;
    if (_nextCard >= NUM_CARDS) {
        *pbEnd = false;
        return lResult;
    }
    *pbEnd = true;

    lResult = _vctCards[_nextCard];

    return lResult;
}
