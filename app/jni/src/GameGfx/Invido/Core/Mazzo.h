#ifndef __CMAZZO_H
#define __CMAZZO_H

#include "CardSpec.h"
#include "ErrorInfo.h"
#include "InvidoCoreDef.h"

namespace invido {

class InvidoCore;

class Mazzo {
   public:
    Mazzo();
    void Init();
    bool Shuffle();
    void Reset() { Init(); }
    bool PickNextCard(CardSpec* pNextCard);
    const std::vector<Uint8>& GetCardIndexOrder() const {
        return _vctCardIndex;
    }
    void SetIndexRaw(int index, Uint8 val);
    LPErrInApp SetCardIndexOrder(const std::vector<Uint8>& cardIndices);
    void SetRandomSeed(int val);

   private:
    std::vector<Uint8> _vctCardIndex;
    int _rndSeed;
};

typedef std::vector<char> VCT_MAZZO;

}  // namespace invido
#endif