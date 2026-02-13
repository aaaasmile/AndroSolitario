#ifndef ___ALGPLAYER_H_
#define ___ALGPLAYER_H_

// TODO change the name

#include "CardSpec.h"
#include "InvidoCoreEnv.h"

class I_CORE_Game {
   public:
    I_CORE_Game() {}
    virtual bool Say(int iPlayerIx, eSayPlayer eSay) = 0;
    virtual bool PlayCard(int iPlayerIx, const CardSpec& cardSpec) = 0;
    virtual bool VaDentro(int iPlayerIx, const CardSpec& cardSpec) = 0;
    virtual void AbandonGame(int iPlayerIx) = 0;
    virtual void GetAdmittedCommands(VCT_COMMANDS& vct_Commands,
                                     int iPlayerIndex) = 0;
    virtual void GetMoreCommands(VCT_COMMANDS& vct_Commands,
                                 int iPlayerIndex) = 0;
};

class I_MatchScore {
   public:
    I_MatchScore() {}
    virtual bool IsGiocatEnd() = 0;
    virtual bool IsGiocataMonte() = 0;
    virtual bool IsManoPatada() = 0;
    virtual bool IsGiocataPatada() = 0;
    virtual bool IsMatchEnd() = 0;
    virtual int GetManoWinner() = 0;
    virtual int GetGiocataWinner() = 0;
    virtual int GetMatchWinner() = 0;
    virtual eGiocataScoreState GetCurrScore() = 0;
    virtual int GetPointsPlayer(int iPlayerIx) = 0;
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
    I_ALG_Player() {}
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore) = 0;
    virtual void ALG_SetPlayerIndex(int iIndex) = 0;
    virtual void ALG_SetOpponentIndex(int iIndex, int iOpponentNr) = 0;
    virtual void ALG_SetAssociateIndex(int iIndex) = 0;
    virtual void ALG_NewMatch(int iNumPlayer) = 0;
    virtual void ALG_NewGiocata(const VCT_CARDSPEC& vctCards, int iPlayerIx) = 0;
    virtual void ALG_PlayerHasPlayed(int iPlayerIx, const CardSpec& cardSpec) = 0;
    virtual void ALG_PlayerHasVadoDentro(int iPlayerIx) = 0;
    virtual void ALG_PlayerHasSaid(int iPlayerIx, eSayPlayer eSay) = 0;
    virtual void ALG_ManoEnd(I_MatchScore* pScore) = 0;
    virtual void ALG_GiocataEnd(I_MatchScore* pScore) = 0;
    virtual void ALG_MatchEnd(I_MatchScore* pScore) = 0;
    virtual void ALG_HaveToPlay() = 0;
    virtual void ALG_HaveToRespond() = 0;
    virtual void ALG_GicataScoreChange(eGiocataScoreState eNewScore) = 0;
    virtual void ALG_PlayerSaidWrong(int iPlayerIx) = 0;
};

#endif
