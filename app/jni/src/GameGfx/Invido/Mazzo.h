#ifndef __CMAZZO_H
#define __CMAZZO_H

#include "CardSpec.h"
#include "InvidoCoreEnv.h"

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
    long PickNextCard(bool* pbEnd);
    bool PickNextCard(CardSpec* pRes);
    long ThrowTableCard();
    void TraceIt();
    void Clear() { _vctCards.clear(); }
    VCT_LONG GetVectorIndexes() { return _vctCards; }
    size_t Count() { return _vctCards.size(); }
    long GetIndexRaw(long l) { return _vctCards[l]; }
    bool IsMoreCards();
    long GetIndexNextCard(bool* pbEnd);
    void SetIndexRaw(int iIndex, long lVal);
    void SetRandomSeed(int iVal) {
        _rndSeed = iVal;
        srand(_rndSeed);
    }

   private:
    VCT_LONG _vctCards;
    long _nextCard;
    InvidoCore* _p_CoreGame;
    int _rndSeed;
};

typedef std::vector<char> VCT_MAZZO;

#endif