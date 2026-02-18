#ifndef __CMAZZO_H
#define __CMAZZO_H

#include "CardSpec.h"
#include "InvidoCoreEnv.h"

namespace invido {

class InvidoCore;

class Mazzo {
   public:
    Mazzo();
    void Create();
    bool Shuffle();
    void Reset() { _nextCard = 0; }
    bool PickNextCard(CardSpec* pNextCard);
    void SetIndexRaw(int iIndex, long lVal);
    void SetRandomSeed(int iVal) {
        _rndSeed = iVal;
        SDL_srand(_rndSeed);
    }

   private:
    VCT_LONG _vctCardIndex;
    size_t _nextCard;
    int _rndSeed;
};

typedef std::vector<char> VCT_MAZZO;

}  // namespace invido
#endif