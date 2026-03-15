#ifndef ___ALGPLAYER_H_
#define ___ALGPLAYER_H_

#include "CardSpec.h"
#include "InvidoCoreDef.h"

namespace invido {

class I_CORE_Game {
   public:
    virtual bool Say(Uint8 playerIx, eSayPlayer eSay) = 0;
    virtual bool PlayCard(Uint8 playerIx, const CardSpec& cardSpec) = 0;
    virtual bool VaDentro(Uint8 playerIx, const CardSpec& cardSpec) = 0;
    virtual void AbandonGame(Uint8 playerIx) = 0;
    virtual void GetAdmittedCommands(VCT_COMMANDS& vct_Commands,
                                     Uint8 playerIx) = 0;
    virtual void GetMoreCommands(VCT_COMMANDS& vct_Commands,
                                 Uint8 playerIx) = 0;
};

class I_MatchScore {
   public:
    //virtual bool IsGiocatEnd() = 0;
    virtual bool IsGiocataMonte() = 0;
    virtual bool IsGiocataPatada() = 0;
    virtual bool IsManoPatada() = 0;
    virtual bool IsMatchEnd() = 0;
    virtual Uint8 GetManoWinner() = 0;
    virtual Uint8 GetGiocataWinner() = 0;
    virtual Uint8 GetMatchWinner() = 0;
    virtual eGiocataScoreState GetCurrScore() = 0;
    virtual int GetPointsPlayer(Uint8 playerIx) = 0;
    virtual int GetManoNum() = 0;
    virtual bool IsGameAbandoned() = 0;
};
/** Inteface to be implemented from player algorithm. Functions are called by
// core and implemented by player. The player table is composed like so:
-------
|1    2|
|0    3|
|------|
The player becomes an index on the virtual game table.
*/
class I_ALG_Player {
   public:
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore) = 0;
    virtual void ALG_SetPlayerIndex(Uint8 playerIx) = 0;
    virtual void ALG_SetOpponentIndex(Uint8 playerIx) = 0;
    virtual void ALG_SetAssociateIndex(Uint8 playerIx) = 0;
    virtual void ALG_NewMatch(int iNumPlayer) = 0;
    virtual void ALG_NewGiocata(const VCT_CARDSPEC& vctCards,
                                Uint8 playerIx) = 0;
    virtual void ALG_PlayerHasPlayed(Uint8 playerIx,
                                     const CardSpec& cardSpec) = 0;
    virtual void ALG_PlayerHasVadoDentro(Uint8 playerIx) = 0;
    virtual void ALG_PlayerHasSaid(Uint8 playerIx, eSayPlayer eSay) = 0;
    virtual void ALG_ManoEnd(I_MatchScore* pScore) = 0;
    virtual void ALG_GiocataEnd(I_MatchScore* pScore) = 0;
    virtual void ALG_MatchEnd(I_MatchScore* pScore) = 0;
    virtual void ALG_HaveToPlay() = 0;
    virtual void ALG_HaveToRespond() = 0;
    virtual void ALG_GicataScoreChange(eGiocataScoreState eNewScore) = 0;
    virtual void ALG_PlayerSaidWrong(Uint8 playerIx) = 0;
};

}  // namespace invido
#endif
