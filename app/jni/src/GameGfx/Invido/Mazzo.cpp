#include "Mazzo.h"

#include <algorithm>
#include <random>

#include "InvidoCore.h"

using namespace invido;

Mazzo::Mazzo() {
    _nextCard = 0;
    _rndSeed = 0;
}

void Mazzo::Init() {
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

void Mazzo::SetIndexRaw(int index, Uint8 val) {
    if (index < _vctCardIndex.size() && index >= 0) {
        _vctCardIndex[index] = val;
    }
}

void Mazzo::SetRandomSeed(int val) { 
    _rndSeed = val; 
    SDL_srand(_rndSeed);
}

bool Mazzo::Shuffle() {
    std::random_device rd;
    if (_rndSeed == 0) {
        std::mt19937 g(rd());
        std::shuffle(_vctCardIndex.begin(), _vctCardIndex.end(), g);
    } else {
        TRACE_DEBUG("Shuffle with predef seed %d \n", _rndSeed);
        std::mt19937 g(_rndSeed);
        std::shuffle(_vctCardIndex.begin(), _vctCardIndex.end(), g);
        _rndSeed += 1;
    }

    return true;
}

bool Mazzo::PickNextCard(CardSpec* pCardPicked) {
    SDL_assert(pCardPicked);
    bool isValid = false;
    if (_nextCard < _vctCardIndex.size()) {
        isValid = true;
        pCardPicked->SetCardIndex(_vctCardIndex[_nextCard]);
        _nextCard++;
    }
    return isValid;
}
