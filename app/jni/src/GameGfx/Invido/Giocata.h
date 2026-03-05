#ifndef ___CGIOCATA____H
#define ___CGIOCATA____H

#include <deque>

#include "InvidoCoreDef.h"
#include "ErrorInfo.h"

namespace invido {
    
class InvidoCore;
class Partita;
class Mano;
class I_MatchScore;
enum eFN_ACTION_GIO { GIOC_START, GIOC_END, GIOC_NO_ACTION };

class ActionItemGio {
   public:
    ActionItemGio() { _eNextAction = GIOC_NO_ACTION; }

    eFN_ACTION_GIO _eNextAction;
    VCT_LONG m_vct_lArg;
};

typedef std::deque<ActionItemGio> DEQ_ACTIT_GIO;

class Giocata {
   public:
    Giocata();

   public:
    void SetCore(InvidoCore* pVal) { _p_InvidoCore = pVal; }
    void SetPartita(Partita* pVal) { _p_Partita = pVal; }
    void SetMano(Mano* pVal) { _p_Mano = pVal; }
    LPErrInApp NewGiocata(Uint8 playerIx);
    void Update_Giocata(Uint8 playerIx, I_MatchScore* pIScore);
    void Reset();
    void NextAction();

   private:
    InvidoCore* _p_InvidoCore;
    Partita* _p_Partita;
    eGiocataStatus _eGiocataStatus;
    Mano* _p_Mano;
    DEQ_ACTIT_GIO _deqNextAction;
};
}

#endif