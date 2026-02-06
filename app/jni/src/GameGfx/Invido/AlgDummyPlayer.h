// AlgDummyPlayer.h: interface for the AlgDummyPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALGDUMMYPLAYER_H__C00C987D_219F_4552_893D_1A67CCACCC8D__INCLUDED_)
#define AFX_CALGDUMMYPLAYER_H__C00C987D_219F_4552_893D_1A67CCACCC8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgPlayerInterface.h"
#include "InvidoCoreEnv.h"
#include "CardSpec.h"

class TraceService;
/** class that implement a dummy player
*/
class AlgDummyPlayer  : public I_ALG_Player
{
public:
	AlgDummyPlayer();
	virtual ~AlgDummyPlayer();

// interface I_ALG_Player
    virtual void ALG_SetCoreInterface(I_CORE_Game* pCore);
    virtual void ALG_SetPlayerIndex(int iIndex) ;
    virtual void ALG_SetOpponentIndex(int iIndex, int iOpponentNr) ;
    virtual void ALG_SetAssociateIndex(int iIndex) ;
    virtual void ALG_NewMatch(int iNumPlayer) ;
    virtual void ALG_NewGiocata(const CARDINFO* pCardArray, int iNumOfCards, int iPlayerIx) ;
    virtual void ALG_PlayerHasPlayed(int iPlayerIx, const CARDINFO* pCard) ;
    virtual void ALG_PlayerHasVadoDentro(int iPlayerIx);
    virtual void ALG_PlayerHasSaid(int iPlayerIx, eSayPlayer eSay) ;
    virtual void ALG_Play() ;
    virtual void ALG_ManoEnd(I_MatchScore* pScore) ;
    virtual void ALG_GiocataEnd(I_MatchScore* pScore) ;
    virtual void ALG_MatchEnd(I_MatchScore* pScore) ;
    virtual void ALG_Say();
    virtual void ALG_GicataScoreChange(eGiocataScoreState eNewScore);
    virtual void ALG_PlayerSaidWrong(int iPlayerIx);

private:
    CardSpec             m_vct_Cards_CPU[NUM_CARDS_HAND];
    CardSpec             m_CardPlayed;
    eGiocataScoreState   m_eScoreCurrent;
    I_CORE_Game*         m_pCoreGame;
    int                  m_iMyIndex;
    int                  m_iOppIndex;
    eSayPlayer           m_OpponentSay;
    eSayPlayer           m_MyLastSay;
    TraceService*        m_pTracer;
};

#endif // !defined(AFX_CALGDUMMYPLAYER_H__C00C987D_219F_4552_893D_1A67CCACCC8D__INCLUDED_)
