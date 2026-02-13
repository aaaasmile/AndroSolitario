#ifndef __CPROBABILITY_H____
#define __CPROBABILITY_H____

#include "CardSpec.h"
#include "InvidoCoreEnv.h"
#include "Mazzo.h"

namespace invido {

typedef std::vector<Uint8> VCT_SINGLECARD;    
typedef std::vector<VCT_SINGLECARD> MTX_HANDCOMBI;

class Probability {
   public:
    Probability() {
        _indexNotInit = true;
        _cardOnHand = 3;
    }
    void SvilCombiHands(VCT_MAZZO& vct_Mazzo, MTX_HANDCOMBI& mtx_Result);
    double BinomialCoef(long lN, long lK);
    double Fattorial(long lN);

    friend std::ostream& operator<<(std::ostream& stream,
                                    const MTX_HANDCOMBI& o);

   private:
    bool _indexNotInit;
    VCT_LONG _vctCounter;
    int _cardOnHand;
};

}

#endif