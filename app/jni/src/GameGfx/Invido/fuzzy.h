#ifndef __FUZZY_COLLECTION_H__
#define __FUZZY_COLLECTION_H__

#include <stdlib.h>

class FuzzyCollection {
   private:
    int fuzzyFALSE;
    int fuzzyTRUE;
    int fuzzyUNDEF;

   public:
    FuzzyCollection()
        : fuzzyFALSE(0), fuzzyTRUE(100), fuzzyUNDEF(fuzzyTRUE >> 1) {}
    FuzzyCollection(int a, int b, int c) {
        fuzzyFALSE = a;
        fuzzyTRUE = b;
        fuzzyUNDEF = c;
    }

    int Fuzzy(int lValue, int lStart, int lEnd);
    int FuzzyAND(int nFirst, int nSecond) {
        return nFirst < nSecond ? nFirst : nSecond;
    }
    int FuzzyOR(int nFirst, int nSecond) {
        return nFirst > nSecond ? nFirst : nSecond;
    }
    int FuzzyHYPEROR(int nFirst, int nSecond) {
        return ((nFirst + nSecond) >= fuzzyTRUE) ? fuzzyTRUE
                                                 : (nFirst + nSecond);
    }
    int FuzzyNOT(int nFuzzy) {
        return nFuzzy < fuzzyTRUE ? (fuzzyTRUE - nFuzzy) : fuzzyFALSE;
    }
};

inline int FuzzyCollection::Fuzzy(int lValue, int lStart, int lEnd) {
    if (lValue <= lStart) {
        return (fuzzyFALSE);
    } else if (lValue >= lEnd) {
        return (fuzzyTRUE);
    } else if (lEnd == lStart) {
        return (fuzzyTRUE);
    } else {
        return ((short)(((lValue - lStart) * fuzzyTRUE) / (lEnd - lStart)));
    }
}

#endif
