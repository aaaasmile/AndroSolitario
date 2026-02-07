#include "Probability.h"

#include <SDL3/SDL.h>

#include <iostream>


void Probability::SvilCombiHands(VCT_MAZZO& vct_Mazzo,
                                 MTX_HANDCOMBI& mtx_Result) {
    long lNumCard = (long)vct_Mazzo.size();
    mtx_Result.clear();

    size_t lNumCombi = (size_t)BinomialCoef(lNumCard, _cardOnHand);

    if (_indexNotInit) {
        _vctCounter.clear();
        // inizializza counter
        for (long i = 0; i < _cardOnHand; i++) {
            _vctCounter.push_back(i);
        }
    }

    _indexNotInit = false;

    // riserva spazio per lo sviluppo integrale
    mtx_Result.reserve(lNumCombi);

    VCT_SINGLECARD vct_tmpRow(_cardOnHand);
    // sviluppo delle colonne
    for (long lCurrRow = 0; lCurrRow < lNumCombi; lCurrRow++) {
        for (long j = 0; j < _cardOnHand; j++) {
            long lIndex = _vctCounter[j];
            SDL_assert(lIndex < lNumCard && lIndex >= 0);
            vct_tmpRow[j] = vct_Mazzo[lIndex];
        }

        mtx_Result.push_back(vct_tmpRow);

        if (lCurrRow == lNumCombi - 1) {
            // ultima colonna, non c'� bisogno di calcolare gli indici
            break;
        }

        long i = 0, j = 0;
        long lChangeSubIndex = 0;
        // incrementa gli indici
        for (j = _cardOnHand - 1; j >= 0; j--) {
            _vctCounter[j]++;
            if (_vctCounter[j] == lNumCard - i) {
                // indice a fine corsa
                lChangeSubIndex = i + 1;
            } else {
                break;
            }
            i++;
        }
        // aggiorna tutti gli indici
        for (; lChangeSubIndex > 0; lChangeSubIndex--) {
            j = _cardOnHand - lChangeSubIndex;
            SDL_assert(j - 1 >= 0);
            SDL_assert(_vctCounter[j - 1] + 1 < lNumCard);
            _vctCounter[j] = _vctCounter[j - 1] + 1;
        }
    }

    // Utility::SaveItemToFile(mtx_Result, DEF_path_s, "allhands.txt");
}

double Probability::BinomialCoef(long lN, long lK) {
    double dResult = 0.0;
    double dNum, dDen;

    dNum = Fattorial(lN);
    dDen = Fattorial(lN - lK) * Fattorial(lK);

    dResult = dNum / dDen;

    return dResult;
}

double Probability::Fattorial(long lN) {
    double dResult = 1.0;

    for (long i = 0; i < lN - 1; i++) {
        dResult *= lN - i;
    }

    return dResult;
}

std::ostream& operator<<(std::ostream& stream, const MTX_HANDCOMBI& o) {
    long lCount = 0;
    size_t iNumEle = o.size();
    for (long i = 0; i < iNumEle; i++) {
        lCount++;
        VCT_SINGLECARD vct_tmpRow;
        vct_tmpRow = o[i];

        if (lCount < 10) {
            stream << " [" << lCount << "] ";
        } else {
            stream << "[" << lCount << "] ";
        }

        size_t iNumRow = vct_tmpRow.size();
        for (long j = 0; j < iNumRow; j++) {
            if (vct_tmpRow[j] < 10) {
                stream << " " << ((int)vct_tmpRow[j]) << " ";
            } else {
                stream << ((int)vct_tmpRow[j]) << " ";
            }
        }
        stream << std::endl;
    }
    return stream;
}
