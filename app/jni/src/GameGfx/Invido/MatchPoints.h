#ifndef __MATCHPOINTS_H
#define __MATCHPOINTS_H

#include <SDL3/SDL.h>

#include "AlgCoreInterface.h"
#include "CardSpec.h"
#include "InvidoCoreDef.h"

namespace invido {

struct ManoInfo {
    Uint8 _playerIndex;
    bool _isPlayed;
    bool _isPata;
};
typedef std::vector<ManoInfo> VCT_MANOINFO;

struct GiocataInfo {
    Uint8 _playerIndex;
    eGiocataScoreState _score;
};
typedef std::vector<GiocataInfo> VCT_GIOCATAINFO;

struct CardPlayed {
    CardSpec _cardSpec;
    Uint8 _playerIx;
};
typedef std::vector<CardPlayed> VCP_CARDPLAYED;

class MatchPoints : public I_MatchScore {
   public:
    MatchPoints();
    virtual ~MatchPoints();
    void MatchStart(int iNumPlayer);

    void GiocataStart();
    void GiocataEndWithWinner(Uint8 playerWinIx);
    void GiocataPatada();
    void GiocataAMonte();

    void PlayerPlay(Uint8 playerIx, const CardSpec& cardSpec);
    void VaDentro(Uint8 playerIx);
    void ManoEnd();
    void ChangeCurrentScore(eGiocataScoreState eVal, int iPlayer);
    void GetManoInfo(int iManoNum, Uint8* pplayerIx, bool* pisPlayed,
                     bool* pisPata);
    void GetGiocataInfo(int iNumGiocata, GiocataInfo* pGiocInfo);
    size_t GetNumGiocateInCurrMatch() { return _vctGiocataInfo.size(); }
    // void PlayerGiocataWins(Uint8 playerWinIx);
    // void SetManoInstance(Mano* pVal) { _p_Mano = pVal; }
    //void AMonte();
    void SetTheWinnerBecauseAbandon(Uint8 playerIx);
    int GetCurrNumCardPlayed() { return _numCardsPlayed; }

    //  I_MatchScore - begin
    bool IsGiocataPatada() { return _isGiocataPatada; }
    bool IsGiocataMonte() { return _isGiocataAMonte; }
    bool IsManoPatada() { return _isManoPatatda; }

    bool IsMatchEnd() { _isMatchEnd; }

    Uint8 GetManoWinner() { return _playerWonsHand; }
    Uint8 GetGiocataWinner() { return _playerGiocataWin; }
    Uint8 GetMatchWinner() { return _playerMatchWin; }

    eGiocataScoreState GetCurrScore() { return _currentScore; }
    int GetPointsPlayer(Uint8 playerIx) {
        SDL_assert(playerIx < _vctPlayerPoints.size());
        return _vctPlayerPoints[playerIx];
    }
    int GetManoNum() { return _manoRound; }
    bool IsGameAbandoned() { return _isGameAbandoned; }
    // I_MatchScore - end

   private:
    void beginSpecialTurn();

   private:
    VCT_INT _vctPlayerPoints;
    VCT_INT _vctHandWons;
    VCT_MANOINFO _vctManoInfo;
    VCT_GIOCATAINFO _vctGiocataInfo;
    VCP_CARDPLAYED _vctCardPlayed;
    eGiocataScoreState _currentScore;
    size_t _numCardsPlayed;
    int _manoRound;
    int _numPlayers;
    int _scoreGoal;

    Uint8 _playerWonsHand;
    Uint8 _playerChangeScore;
    Uint8 _playerFirstHand;
    Uint8 _playerGiocataWin;
    Uint8 _playerMatchWin;

    // Mano* _p_Mano;
    bool _isMatchInSpecialScore;
    bool _isOldManoPatada;
    bool _isGameAbandoned;
    bool _isGiocataPatada;
    bool _isGiocataAMonte;
    bool _isMatchEnd;
    bool _isManoPatatda;
};

}  // namespace invido

#endif
