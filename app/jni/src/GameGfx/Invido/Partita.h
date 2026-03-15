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
    enum eFN_ACTION { ACT_PARTITA_END, NO_ACTION };

   public:
    Partita();

   public:
    void SetPartitaCB(PartitaCb& partitaCb) { _partitaCb = partitaCb; }
    LPErrInApp NewPartita(Uint8 playerStartIx);
    LPErrInApp Update_Partita(I_MatchScore* pIScore);
    void Reset();
    void NextAction();
    bool IsOngoing() {
        return _PartitaState == ePartitaStatus::PARTITA_ONGOING;
    }

   private:
    ePartitaStatus _PartitaState;
    PartitaCb _partitaCb;
    Uint8 _playerStartIx;
    eFN_ACTION _eNextAction;
};

}  // namespace invido
#endif
