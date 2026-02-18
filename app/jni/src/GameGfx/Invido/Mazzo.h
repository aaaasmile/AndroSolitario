#ifndef __CMAZZO_H
#define __CMAZZO_H

#include "CardSpec.h"
#include "InvidoCoreEnv.h"

namespace invido {

class InvidoCore;

class Mazzo {
   public:
    Mazzo();
    void SetCoreEngine(InvidoCore* pVal) { _p_CoreGame = pVal; }
    void Create();
    bool CloneFrom(Mazzo& Master);
    long GetNextCardVal() { return _nextCard; }
    bool Shuffle();
    void Reset() { _nextCard = 0; }
    CardSpec* PickNextCard(bool* pIsValid);
    void SetIndexRaw(int iIndex, long lVal);
    void SetRandomSeed(int iVal) {
        _rndSeed = iVal;
        SDL_srand(_rndSeed);
    }

   private:
    VCT_LONG _vctCardIndex;
    CardSpec _arrCardSpec[NUM_CARDS];
    size_t _nextCard;
    InvidoCore* _p_CoreGame;
    int _rndSeed;
};

typedef std::vector<char> VCT_MAZZO;

}
#endif