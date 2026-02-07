#ifndef __CPROBABILITY_H____
#define __CPROBABILITY_H____

#include "CardSpec.h"
#include "InvidoCoreEnv.h"
#include "Mazzo.h"

typedef std::vector<VCT_SINGLECARD> MTX_HANDCOMBI;

class Probality {
   public:
    Probality() {
        m_bIndexNotInit = true;
        m_iCardOnHand = 3;
    }
    void SvilCombiHands(VCT_MAZZO& vct_Mazzo, MTX_HANDCOMBI& mtx_Result);
    double BinomialCoef(long lN, long lK);
    double Fattorial(long lN);

    friend std::ostream& operator<<(std::ostream& stream,
                                    const MTX_HANDCOMBI& o);

   private:
    bool m_bIndexNotInit;
    VCT_LONG m_vctCounter;
    int m_iCardOnHand;
};

#endif