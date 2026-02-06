

// Giocata.h

#ifndef ___CGIOCATA____H
#define ___CGIOCATA____H

#include "InvidoCoreEnv.h"
#include <deque>

class InvidoCore;
class Partita;
class Mano;
class I_MatchScore;

//! enum action type
enum eFN_ACTION_GIO
{
    //! giocata start
    GIOC_START,
    //! giocata end
    GIOC_END,
    //! no action
    GIOC_NO_ACTION
};

//! class ActionItemGio
/** action item in the queue of giocata tasks
*/
class ActionItemGio
{
public:
    //! constructor
    ActionItemGio(){m_eNextAction = GIOC_NO_ACTION;}

    eFN_ACTION_GIO      m_eNextAction;
    //! argument list for action calls
    VCT_LONG            m_vct_lArg;
};

typedef std::deque< ActionItemGio > DEQ_ACTIT_GIO;


/////////////////////////////////////////////////////////////////////////////////////
//   *******************  CGIOCATA CLASS ***************************************
/////////////////////////////////////////////////////////////////////////////////////

//! class Giocata
/**
Manage a giocata on invido. There is 3 mano on one giocata.
*/
class Giocata
{
public:
    Giocata();
public:
    void    SetCore(InvidoCore* pVal){m_pInvidoCore = pVal;}
    void    SetPartita(Partita* pVal){m_pPartita = pVal;}
    void    SetMano(Mano* pVal){m_pMano = pVal;}
    //! start a new giocata
    void    NewGiocata(long lPlayerIx);
    void    Update_Giocata(long lPlayerIx, I_MatchScore* pIScore);
    void    Reset();
    //! do the next action
    void    NextAction();

private:
    InvidoCore*     m_pInvidoCore;
    Partita*        m_pPartita;
    eGiocataStatus   m_eGiocataStatus;
    Mano*           m_pMano;
    //! next action queue
    DEQ_ACTIT_GIO    m_deqNextAction;
};


#endif