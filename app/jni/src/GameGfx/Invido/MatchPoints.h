#ifndef __MATCHPOINTS_H
#define __MATCHPOINTS_H

#include <SDL3/SDL.h>

#include "AlgCoreInterface.h"
#include "CardSpec.h"
#include "InvidoCoreDef.h"

namespace invido {

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

class CardPlayed {
   public:
    CardPlayed() {}
    CardSpec cardSpec;
    int playerIx;
};

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
    void PlayerPlay(int iPlayerIx, const CardSpec& cardSpec);
    void VaDentro(int iPlayerIx);
    void ManoEnd();
    void ChangeCurrentScore(eGiocataScoreState eVal, int iPlayer);
    void GetManoInfo(int iManoNum, int* piPlayerIx, bool* pbIsPlayed,
                     bool* pbIsPata);
    void GetGiocataInfo(int iNumGiocata, GiocataInfo* pGiocInfo);
    int GetNumGiocateInCurrMatch() { return (int)_vctGiocataInfo.size(); }
    void PlayerVaVia(int iPlayerIx);
    void SetManoInstance(Mano* pVal) { _p_Mano = pVal; }
    void AMonte();
    void SetTheWinner(int iPlayerIx);
    int GetCurrNumCardPlayed() { return _numCardsPlayed; }

    // interface I_MatchScore - begin
    virtual bool IsGiocatEnd();
    virtual bool IsGiocataMonte() {
        if (_eIsGiocataEnd == GES_AMONTE)
            return true;
        else
            return false;
    }
    virtual bool IsManoPatada() { return _isManoPatatda; }
    virtual bool IsGiocataPatada() {
        if (_eIsGiocataEnd == GES_PATADA)
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
    virtual int GetManoWinner() { return _playerWonsHand; }
    virtual int GetGiocataWinner() { return _playerGiocataWin; }
    virtual int GetMatchWinner() { return _playerMatchWin; }
    virtual eGiocataScoreState GetCurrScore() { return _currentScore; }
    virtual int GetPointsPlayer(Uint8 playerIx) {
        SDL_assert(playerIx >= 0 && playerIx < MAX_NUM_PLAYER);
        return _vctPlayerPoints[playerIx];
    }
    virtual int GetManoNum() { return _manoRound; }
    virtual bool IsGameAbandoned() { return _isGameAbandoned; }
    // interface I_MatchScore - end

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

}  // namespace invido

#endif
