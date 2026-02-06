

// MatchPoints.h: interface for the MatchPoints class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( \
    AFX_CMATCHPOINTS_H__A8E0031A_CF94_4322_9E51_9C761FDDDC7C__INCLUDED_)
#define AFX_CMATCHPOINTS_H__A8E0031A_CF94_4322_9E51_9C761FDDDC7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <deque>

#include "CardSpec.h"
#include "InvidoInfoComm.h"
#include "InvidoCoreEnv.h"
#include "win_type_global.h"


class Mano;
class ManoInfo {
   public:
    ManoInfo() {}
    void Reset() {
        bIsPlayed = FALSE;
        bIsPata = FALSE;
        iPlayerIndex = NOT_VALID_INDEX;
    }
    int iPlayerIndex;
    bool bIsPlayed;
    bool bIsPata;
};
class GiocataInfo {
   public:
    GiocataInfo() {
        eScore = SC_AMONTE;
        iPlayerIndex = NOT_VALID_INDEX;
    }
    GiocataInfo(int iVal, eGiocataScoreState eVal) {
        eScore = eVal;
        iPlayerIndex = iVal;
    }
    int iPlayerIndex;
    eGiocataScoreState eScore;
};

typedef std::vector<GiocataInfo> VCT_GIOCATAINFO;
// information about played card
class CardPlayed {
   public:
    CardPlayed() {}
    CardSpec cardSpec;
    int iPlayerIx;
};
/** class that provides match points informations. All informations are about
 * the current match
 */
class MatchPoints : public I_MatchScore {
    enum eGiocataEndState {
        GES_ON_GOING,
        GES_HAVE_WINNER,
        GES_PATADA,
        GES_AMONTE
    };

   public:
    MatchPoints();
    virtual ~MatchPoints();
    void MatchStart(int iNumPlayer);
    void GiocataStart();
    void GiocataEnd();
    void PlayerPlay(int iPlayerIx, CARDINFO* pCard);
    void ManoEnd();
    void ChangeCurrentScore(eGiocataScoreState eVal, int iPlayer);
    void GetManoInfo(int iManoNum, int* piPlayerIx, bool* pbIsPlayed,
                     bool* pbIsPata);
    void GetGiocataInfo(int iNumGiocata, GiocataInfo* pGiocInfo);
    int GetNumGiocateInCurrMatch() { return (int)m_vctGiocataInfo.size(); }
    void PlayerVaVia(int iPlayerIx);
    void SetManoObj(Mano* pVal) { m_pMano = pVal; }
    void AMonte();
    void SetTheWinner(int iPlayerIx);
    int GetCurrNumCardPlayed() { return m_iNumCardsPlayed; }

    // interface I_MatchScore
    virtual int GetManoWinner() { return m_iPlayerWonsHand; }
    virtual int GetGiocataWinner() { return m_iPlayerGiocataWin; }
    virtual int GetMatchWinner() { return m_iPlayerMatchWin; }
    virtual bool IsGiocatEnd();
    virtual eGiocataScoreState GetCurrScore() { return m_eCurrentScore; }
    virtual bool IsManoPatada() { return m_bIsManoPatatda; }
    virtual bool IsGiocataPatada() {
        if (m_eIsGiocataEnd == GES_PATADA)
            return TRUE;
        else
            return FALSE;
    }
    virtual bool IsGiocataMonte() {
        if (m_eIsGiocataEnd == GES_AMONTE)
            return TRUE;
        else
            return FALSE;
    }
    virtual bool IsMatchEnd() {
        if (m_iPlayerMatchWin != NOT_VALID_INDEX)
            return TRUE;
        else
            return FALSE;
    }
    virtual int GetPointsPlayer(int iPlayerIx) {
        ASSERT(iPlayerIx >= 0 && iPlayerIx < MAX_NUM_PLAYER);
        return m_vctPlayerPoints[iPlayerIx];
    }
    virtual int GetManoNum() { return m_iManoRound; }
    virtual bool IsGameAbandoned() { return m_bGameAbandoned; }

   private:
    void beginSpecialTurn();

   private:
    int m_vctPlayerPoints[MAX_NUM_PLAYER];
    int m_vctHandWons[MAX_NUM_PLAYER];
    eGiocataScoreState m_eCurrentScore;
    int m_iPlayerChangeScore;
    CardPlayed m_vctCardPlayed[MAX_NUM_PLAYER];
    int m_iNumCardsPlayed;
    int m_iPlayerWonsHand;
    int m_iNumPlayers;
    bool m_bIsManoPatatda;
    eGiocataEndState m_eIsGiocataEnd;
    int m_iPlayerFirstHand;
    int m_iManoRound;
    int m_iPlayerGiocataWin;
    int m_iPlayerMatchWin;
    int m_iScoreGoal;
    bool m_bMatchInSpecialScore;
    bool m_bOldManoPatada;
    ManoInfo m_ManoDetailInfo[NUM_CARDS_HAND];
    VCT_GIOCATAINFO m_vctGiocataInfo;
    Mano* m_pMano;
    bool m_bGameAbandoned;
};

#endif  // !defined(AFX_CMATCHPOINTS_H__A8E0031A_CF94_4322_9E51_9C761FDDDC7C__INCLUDED_)
