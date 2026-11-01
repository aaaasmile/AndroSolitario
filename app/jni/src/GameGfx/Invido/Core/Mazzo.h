#ifndef __CMAZZO_H
#define __CMAZZO_H

#include "CardSpec.h"
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
    void SetIndexRaw(int index, Uint8 val);
    void SetRandomSeed(int val);

   private:
    std::vector<Uint8> _vctCardIndex;
    int _rndSeed;
};

typedef std::vector<char> VCT_MAZZO;

}  // namespace invido
#endif