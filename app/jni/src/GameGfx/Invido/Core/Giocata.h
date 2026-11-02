#ifndef ___CGIOCATA____H
#define ___CGIOCATA____H

#include <deque>

#include "ErrorInfo.h"
#include "InvidoCoreDef.h"
#include "InvidoTraits.h"

namespace invido {

class InvidoCore;
class Partita;
class Mano;
class I_MatchScore;

class Giocata {
    enum eGiocataEndState {
        GES_ON_GOING,
        GES_HAVE_WINNER,
        GES_PATADA,
        GES_AMONTE
    };
    enum eFN_ACTION_GIO {
        GIOC_STARTED,
        GIOC_WITHWIN_ENDED,
        GIOC_NOWINNERS_ENDED
    };

    struct ActionItemGio {
        eFN_ACTION_GIO _eNextAction;
        Uint8 _playerIx;
    };
    typedef std::deque<ActionItemGio> DEQ_ACTIT_GIO;

   public:
    Giocata();

   public:
    void SetGiocataCB(GiocataCb& giocataCb) { _giocataCb = giocataCb; }
    LPErrInApp NewGiocata(Uint8 playerIx);

    // void Update_Giocata();
    void Reset();
    LPErrInApp NextAction(bool& hasAction);
    void PlayerGiocataWins(Uint8 playerWinnerIx);
    void GiocataAMonte();
    void GiocataPatada();

   private:
    GiocataCb _giocataCb;
    eGiocataStatus _eGiocataStatus;
    DEQ_ACTIT_GIO _deqNextAction;
    int _numGiocata;
};
}  // namespace invido

#endif