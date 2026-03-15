#ifndef __CPARTITA_H__
#define __CPARTITA_H__

#include "ErrorInfo.h"
#include "InvidoCoreDef.h"
#include "InvidoTraits.h"

namespace invido {

class InvidoCore;
class Giocata;
class I_MatchScore;

class Partita {
    enum eFN_ACTION_PARTITA { ACT_PARTITA_STARTED, ACT_PARTITA_END };

    struct ActionItemPartita {
        eFN_ACTION_PARTITA _eNextAction;
        Uint8 _playerIx;
    };
    typedef std::deque<ActionItemPartita> DEQ_ACT_PARTITA;

   public:
    Partita();

   public:
   bool IsOngoing() {return _PartitaState == PARTITA_ONGOING;}
    void SetPartitaCB(PartitaCb& partitaCb) { _partitaCb = partitaCb; }
    LPErrInApp NewPartita(Uint8 playerStartIx);
    void PartitaEnd();
    void Reset();
    void NextAction();

   private:
    ePartitaStatus _PartitaState;
    PartitaCb _partitaCb;
    DEQ_ACT_PARTITA _deqNextAction;
};

}  // namespace invido
#endif
