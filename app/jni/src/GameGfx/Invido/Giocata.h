#ifndef ___CGIOCATA____H
#define ___CGIOCATA____H

#include <deque>

#include "InvidoCoreEnv.h"
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
    void SetCore(InvidoCore* pVal) { m_pInvidoCore = pVal; }
    void SetPartita(Partita* pVal) { m_pPartita = pVal; }
    void SetMano(Mano* pVal) { m_pMano = pVal; }
    void NewGiocata(long lPlayerIx);
    void Update_Giocata(long lPlayerIx, I_MatchScore* pIScore);
    void Reset();
    void NextAction();

   private:
    InvidoCore* m_pInvidoCore;
    Partita* m_pPartita;
    eGiocataStatus m_eGiocataStatus;
    Mano* m_pMano;
    DEQ_ACTIT_GIO m_deqNextAction;
};
}

#endif