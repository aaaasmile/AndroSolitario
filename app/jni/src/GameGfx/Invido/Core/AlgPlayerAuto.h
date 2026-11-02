#ifndef INVIDO_ALG_PLAYER_AUTO_H
#define INVIDO_ALG_PLAYER_AUTO_H

#include "AlgCoreInterface.h"
#include "GameActionReplayer.h"

namespace invido {

class AlgPlayerAuto : public I_ALG_Player {
   public:
    AlgPlayerAuto(GameActionReplayer& replayer, Uint8 playerIndex);
    virtual ~AlgPlayerAuto();

    virtual void ALG_SetCoreInterface(I_CORE_Game* core);
    virtual void ALG_SetPlayerIndex(Uint8 playerIndex);
    virtual void ALG_SetOpponentIndex(Uint8 playerIndex);
    virtual void ALG_SetAssociateIndex(Uint8 playerIndex);
    virtual void ALG_NewMatch(int playerCount);
    virtual void ALG_NewGiocata(const VCT_CARDSPEC& cards, Uint8 playerIndex);
    virtual void ALG_PlayerHasPlayed(Uint8 playerIndex, const CardSpec& card);
    virtual void ALG_PlayerHasVadoDentro(Uint8 playerIndex);
    virtual void ALG_PlayerHasSaid(Uint8 playerIndex, eSayPlayer say);
    virtual void ALG_ManoEnd(I_MatchScore* score);
    virtual void ALG_GiocataEnd(I_MatchScore* score);
    virtual void ALG_MatchEnd(I_MatchScore* score);
    virtual LPErrInApp ALG_HaveToPlay();
    virtual LPErrInApp ALG_HaveToRespond();
    virtual void ALG_GicataScoreChange(eGiocataScoreState newScore);
    virtual void ALG_PlayerSaidWrong(Uint8 playerIndex);

   private:
    LPErrInApp PlayNextRecordedAction(GameActionReplayer::PlayerAction::Type expectedType);
    
    GameActionReplayer& _replayer;
    I_CORE_Game* _p_CoreGame;
    Uint8 _playerIndex;
};

}  // namespace invido

#endif
