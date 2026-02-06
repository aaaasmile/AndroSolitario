

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

//! class Partita
/** class to manage a new partita
*/
class Partita
{
//! enum action type
enum eFN_ACTION
{
    //! partita end
    ACT_PARTITA_END,
    //! no action
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
    //! do the next action
    void    NextAction();

private:
    ePartitaStatus   m_PartitaState;
    InvidoCore*     m_pInvidoCore;
    Giocata*        m_pGiocata;
    long             m_lGiocStart;
    //! next action 
    eFN_ACTION         m_eNextAction;
};


#endif
