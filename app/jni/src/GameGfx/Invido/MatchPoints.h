#ifndef __MATCHPOINTS_H
#define __MATCHPOINTS_H

#include <deque>

#include "CardSpec.h"
#include "InvidoCoreEnv.h"
#include "InvidoInfoComm.h"
#include "win_type_global.h"

class Mano;
class ManoInfo {
   public:
    ManoInfo() {}
    void Reset() {
        isPlayed = false;
        isPata = false;
        playerIndex = NOT_VALID_INDEX;
    }
    int playerIndex;
    bool isPlayed;
    bool isPata;
};
class GiocataInfo {
   public:
    GiocataInfo() {
        score = SC_AMONTE;
        playerIndex = NOT_VALID_INDEX;
    }
    GiocataInfo(int iVal, eGiocataScoreState eVal) {
        score = eVal;
        playerIndex = iVal;
    }
    int playerIndex;
    eGiocataScoreState score;
};

typedef std::vector<GiocataInfo> VCT_GIOCATAINFO;
// information about played card
class CardPlayed {
   public:
    CardPlayed() {}
    CardSpec cardSpec;
    int playerIx;
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
    int GetNumGiocateInCurrMatch() { return (int)_vctGiocataInfo.size(); }
    void PlayerVaVia(int iPlayerIx);
    void SetManoObj(Mano* pVal) { _p_Mano = pVal; }
    void AMonte();
    void SetTheWinner(int iPlayerIx);
    int GetCurrNumCardPlayed() { return _numCardsPlayed; }

    // interface I_MatchScore
    virtual int GetManoWinner() { return _playerWonsHand; }
    virtual int GetGiocataWinner() { return _playerGiocataWin; }
    virtual int GetMatchWinner() { return _playerMatchWin; }
    virtual bool IsGiocatEnd();
    virtual eGiocataScoreState GetCurrScore() { return _currentScore; }
    virtual bool IsManoPatada() { return _isManoPatatda; }
    virtual bool IsGiocataPatada() {
        if (_eIsGiocataEnd == GES_PATADA)
            return true;
        else
            return false;
    }
    virtual bool IsGiocataMonte() {
        if (_eIsGiocataEnd == GES_AMONTE)
            return true;
        else
            return false;
    }
    virtual bool IsMatchEnd() {
        if (_playerMatchWin != NOT_VALID_INDEX)
            return true;
        else
            return false;
    }
    virtual int GetPointsPlayer(int iPlayerIx) {
        SDL_assert(iPlayerIx >= 0 && iPlayerIx < MAX_NUM_PLAYER);
        return _vctPlayerPoints[iPlayerIx];
    }
    virtual int GetManoNum() { return _manoRound; }
    virtual bool IsGameAbandoned() { return _isGameAbandoned; }

   private:
    void beginSpecialTurn();

   private:
    int _vctPlayerPoints[MAX_NUM_PLAYER];
    int _vctHandWons[MAX_NUM_PLAYER];
    eGiocataScoreState _currentScore;
    int _playerChangeScore;
    CardPlayed _vctCardPlayed[MAX_NUM_PLAYER];
    int _numCardsPlayed;
    int _playerWonsHand;
    int _numPlayers;
    bool _isManoPatatda;
    eGiocataEndState _eIsGiocataEnd;
    int _playerFirstHand;
    int _manoRound;
    int _playerGiocataWin;
    int _playerMatchWin;
    int _scoreGoal;
    bool _isMatchInSpecialScore;
    bool _isOldManoPatada;
    ManoInfo _ManoDetailInfo[NUM_CARDS_HAND];
    VCT_GIOCATAINFO _vctGiocataInfo;
    Mano* _p_Mano;
    bool _isGameAbandoned;
};

#endif  // !defined(AFX_CMATCHPOINTS_H__A8E0031A_CF94_4322_9E51_9C761FDDDC7C__INCLUDED_)
