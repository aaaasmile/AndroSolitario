

// Partita.h

#ifndef __CPARTITA_H__
#define __CPARTITA_H__

#include "InvidoCoreEnv.h"

class InvidoCore;
class Giocata;
class I_MatchScore;

/////////////////////////////////////////////////////////////////////////////////////
//   *******************  CPARTITA CLASS ***************************************
/////////////////////////////////////////////////////////////////////////////////////
/** class to manage a new partita
*/
class Partita
{
enum eFN_ACTION
{
    ACT_PARTITA_END,
    NO_ACTION
};
public:
    Partita();

public:
    void    SetCore(InvidoCore* pVal){m_pInvidoCore = pVal;}
    void    SetGiocata(Giocata* pVal){m_pGiocata = pVal;}
    void    NewPartita(long lPlayerIx);
    void    Update_Partita(I_MatchScore* pIScore);
    void    Reset();
    void    NextAction();

private:
    ePartitaStatus   m_PartitaState;
    InvidoCore*     m_pInvidoCore;
    Giocata*        m_pGiocata;
    long             m_lGiocStart;
    eFN_ACTION         m_eNextAction;
};


#endif
