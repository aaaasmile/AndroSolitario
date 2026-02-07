

// Mazzo.h

#ifndef __CMAZZO_H
#define __CMAZZO_H

#include "CardSpec.h"
#include "InvidoCoreEnv.h"
#include "win_type_global.h"

class InvidoCore;

class Mazzo {
   public:
    Mazzo();
    void SetCoreEngine(InvidoCore* pVal) { m_pCoreGame = pVal; }
    void Create();
    bool CloneFrom(Mazzo& Master);
    long GetNextCardVal() { return m_lNextCard; }
    bool Shuffle();
    void Reset() { m_lNextCard = 0; }
    long PickNextCard(bool* pbEnd);
    bool PickNextCard(CardSpec* pRes);
    long ThrowTableCard();
    void TraceIt();
    void Clear() { m_vctCards.clear(); }
    VCT_LONG GetVectorIndexes() { return m_vctCards; }
    size_t Count() { return m_vctCards.size(); }
    long GetIndexRaw(long l) { return m_vctCards[l]; }
    bool IsMoreCards();
    long GetIndexNextCard(bool* pbEnd);
    void SetIndexRaw(int iIndex, long lVal);
    void SetRandomSeed(int iVal) {
        m_iRndSeed = iVal;
        srand(m_iRndSeed);
    }

   private:
    VCT_LONG m_vctCards;
    long m_lNextCard;
    InvidoCore* m_pCoreGame;
    int m_iRndSeed;
};

typedef std::vector<char> VCT_MAZZO;

#endif