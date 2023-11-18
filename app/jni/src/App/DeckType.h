#ifndef _DECKTYPE_H_
#define _DECKTYPE_H_

#include <string>

#include "ErrorInfo.h"

enum eDeckType {
    PIACENTINA = 0,
    BERGAMO,
    BOLOGNA,
    GENOVA,
    MILANO,
    NAPOLI,
    PIEMONTE,
    ROMAGNA,
    SARDEGNA,
    SICILIA,
    TOSCANA,
    TRENTO,
    TREVISO,
    TRIESTE,
    TAROCK_PIEMONT,
    NUM_OF_DECK
};

class DeckType {
public:
    enum ePacType { PacTypePac, PacTypeSingleFile };

    DeckType() {}

    LPErrInApp SetType(eDeckType eType);
    std::string GetDeckName() { return _strDeckName; }
    std::string GetResFileName() { return _strResFileName; }
    std::string GetSymbolFileName() { return _strSymbolFileName; }
    eDeckType GetType() { return _eType; }
    ePacType GetPacType() { return _ePacType; }
    bool IsPacType() { return _ePacType == PacTypePac; }
    void CopyFrom(DeckType& Val);
    LPErrInApp SetTypeIndex(int iVal);
    int GetTypeIndex() { return (int)_eType; }
    int GetNumCardInSuit();
    int GetNumCards();
    int GetMaxRank();
    int GetRank(int ix);

private:
    eDeckType _eType;
    ePacType _ePacType;
    std::string _strDeckName;
    std::string _strSymbolFileName;
    std::string _strResFileName;
};

#endif