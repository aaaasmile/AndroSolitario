#include "MatchPoints.h"

#include "Mano.h"

using namespace invido;

MatchPoints::MatchPoints() {
    _numPlayers = NUM_PLAY_INVIDO_2;
    _p_Mano = 0;
    _isGameAbandoned = false;
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        _vctManoInfo[iManoNum].Reset();
    }
    _currentScore = SC_CANELA;
}

MatchPoints::~MatchPoints() {}

void MatchPoints::MatchStart(int iNumPlayer) {
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        _vctPlayerPoints[i] = 0;
    }
    _numPlayers = iNumPlayer;
    _playerMatchWin = NOT_VALID_INDEX;
    _scoreGoal = SCORE_GOAL;
    _isMatchInSpecialScore = false;
    _vctGiocataInfo.clear();
}

void MatchPoints::GiocataStart() {
    _vctHandWons.clear();
    _vctCardPlayed.clear();
    for (int i = 0; i < _numPlayers; i++) {
        _vctHandWons.push_back(0);
    }
    _numCardsPlayed = 0;
    _manoRound = 0;
    _currentScore = SC_CANELA;
    _playerWonsHand = NOT_VALID_INDEX;
    _playerFirstHand = NOT_VALID_INDEX;
    _playerGiocataWin = NOT_VALID_INDEX;

    
    _vctManoInfo.clear();
    _isGameAbandoned = false;
    _isGiocataAMonte = false;
    _isGiocataPatada = false;
    _isOldManoPatada = false;
    _isManoPatatda = false;
}

void MatchPoints::PlayerPlay(Uint8 playerIx, const CardSpec& cardSpec) {
    SDL_assert(_numCardsPlayed < _vctCardPlayed.size());
    _vctCardPlayed[_numCardsPlayed]._playerIx = playerIx;
    _vctCardPlayed[_numCardsPlayed]._cardSpec = cardSpec;
    _numCardsPlayed++;
}

void MatchPoints::VaDentro(Uint8 playerIx) {
    CardSpec cardSpec;
    cardSpec.SetCardIndex(4);  // make sure that the card will always loose
    SDL_assert(_numCardsPlayed < _vctCardPlayed.size());

    _vctCardPlayed[_numCardsPlayed]._playerIx = playerIx;
    _vctCardPlayed[_numCardsPlayed]._cardSpec.SetCardIndex(
        cardSpec.GetCardIndex());
    _numCardsPlayed++;
}

void MatchPoints::ManoEnd() {
    SDL_assert(_numCardsPlayed == _numPlayers);
    int iManoTerminatedIndex = _manoRound;
    SDL_assert(iManoTerminatedIndex >= 0 &&
               iManoTerminatedIndex < _vctManoInfo.size());
    _manoRound++;
    // hand is terminated
    if (_numPlayers == NUM_PLAY_INVIDO_2)  // consistency check
    {
        int iPoints_1 = _vctCardPlayed[PLAYER1]._cardSpec.GetPoints();
        int iPlayer_1 = _vctCardPlayed[PLAYER1]._playerIx;
        int iPoints_2 = _vctCardPlayed[PLAYER2]._cardSpec.GetPoints();
        int iPlayer_2 = _vctCardPlayed[PLAYER2]._playerIx;

        _isManoPatatda = false;
        // mano is played
        _vctManoInfo[iManoTerminatedIndex]._isPlayed = true;

        if (iPoints_1 == iPoints_2) {
            // nobody wins the hand
            _isManoPatatda = true;
            if (_playerFirstHand != NOT_VALID_INDEX) {
                // the first player who take the mano is the giocata winner
                _playerGiocataWin = _playerFirstHand;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = NOT_VALID_INDEX;
            _vctManoInfo[iManoTerminatedIndex]._isPata = true;

            // mark the mano patada, the next who take the trick win
            _isOldManoPatada = true;
        } else if (iPoints_1 > iPoints_2) {
            // first player wons the mano
            _vctHandWons[iPlayer_1]++;
            if (_playerFirstHand == NOT_VALID_INDEX) {
                _playerFirstHand = iPlayer_1;
            }
            if (_vctHandWons[iPlayer_1] >= NUM_PLAY_INVIDO_2 ||
                _isOldManoPatada) {
                // giocata is terminated, giocata winner is the first player in
                // this hand
                _playerGiocataWin = iPlayer_1;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = iPlayer_1;
            _vctManoInfo[iManoTerminatedIndex]._playerIndex = iPlayer_1;
        } else {
            // second player catch the mano
            _vctHandWons[iPlayer_2]++;
            if (_playerFirstHand == NOT_VALID_INDEX) {
                _playerFirstHand = iPlayer_2;
            }
            if (_vctHandWons[iPlayer_2] >= NUM_PLAY_INVIDO_2 ||
                _isOldManoPatada) {
                // giocata is terminated, giocata winner is the second player in
                // this hand
                _playerGiocataWin = iPlayer_2;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = iPlayer_2;
            _vctManoInfo[iManoTerminatedIndex]._playerIndex = iPlayer_2;
        }
        if (_manoRound >= NUM_CARDS_HAND) {
            // giocata is terminated
            if (_isManoPatatda && (_playerFirstHand == NOT_VALID_INDEX)) {
                // strange case all hands was patada. Giocata is also patada.
                _eIsGiocataEnd = GES_PATADA;
                _playerGiocataWin = iPlayer_1;
            } else {
                // giocata winner must be already defined
                SDL_assert(_playerGiocataWin != NOT_VALID_INDEX);
            }
        }
    } else {
        SDL_assert(0);
    }
    _numCardsPlayed = 0;
}

void MatchPoints::GiocataPatada() {
    GiocataInfo gi = {._score = SC_PATTA};
    _vctGiocataInfo.push_back(gi);
    _vctManoInfo.clear();
    _isGiocataPatada = true;
}

void MatchPoints::GiocataAMonte() {
    GiocataInfo gi = {._score = SC_AMONTE};
    _vctGiocataInfo.push_back(gi);
    _vctManoInfo.clear();
    _isGiocataAMonte = true;
}

void MatchPoints::GiocataEndWithWinner(Uint8 playerWinIx) {
    // if (_eIsGiocataEnd == GES_HAVE_WINNER) {
    // update the score
    SDL_assert(playerWinIx < _vctPlayerPoints.size());
    _playerGiocataWin = playerWinIx;
    _vctPlayerPoints[_playerGiocataWin] += _currentScore;
    GiocataInfo gi = {._playerIndex = _playerGiocataWin,
                      ._score = _currentScore};
    _vctGiocataInfo.push_back(gi);

    if (_vctPlayerPoints[_playerGiocataWin] >= _scoreGoal) {
        // match is terminated
        _playerMatchWin = _playerGiocataWin;
    } else if (_vctPlayerPoints[PLAYER1] == SPECIAL_SCORE &&
               _vctPlayerPoints[PLAYER2] == SPECIAL_SCORE) {
        // special condition both player are on 23 to 23 or after 7 to 7....
        beginSpecialTurn();
    } else if (_isMatchInSpecialScore &&
               (_vctPlayerPoints[PLAYER1] == SCORE_SEVEN &&
                _vctPlayerPoints[PLAYER2] == SCORE_SEVEN)) {
        beginSpecialTurn();
    }
    // } else {
    //     // pata or monte
    //     _vctGiocataInfo.push_back(GiocataInfo(NOT_VALID_INDEX, SC_AMONTE));
    // }
    // for (ManoInfo& manoInfo : _vctManoInfo) {
    //     manoInfo.Reset();
    // }
    _vctManoInfo.clear();
}

void MatchPoints::beginSpecialTurn() {
    _scoreGoal = SPECIAL_SCORE_GOAL;
    _vctPlayerPoints[PLAYER1] = 0;
    _vctPlayerPoints[PLAYER2] = 0;
    _isMatchInSpecialScore = true;
}

void MatchPoints::GetManoInfo(int manoNum, Uint8* pplayerIx, bool* pisPlayed,
                     bool* pisPata) {
    SDL_assert(pisPata);
    SDL_assert(pisPlayed);
    SDL_assert(pplayerIx);
    if (manoNum >= 0 && manoNum < NUM_CARDS_HAND) {
        *pisPata = _vctManoInfo[manoNum]._isPata;
        *pisPlayed = _vctManoInfo[manoNum]._isPlayed;
        *pplayerIx = _vctManoInfo[manoNum]._playerIndex;

    } else {
        SDL_assert(0);
    }
}

void MatchPoints::GetGiocataInfo(int iNumGiocata, GiocataInfo* pGiocInfo) {
    SDL_assert(pGiocInfo);
    if (iNumGiocata >= 0 && iNumGiocata < (int)_vctGiocataInfo.size()) {
        *pGiocInfo = _vctGiocataInfo[iNumGiocata];
    }
}

void MatchPoints::ChangeCurrentScore(eGiocataScoreState eVal, int iPlayer) {
    if (eVal > 0) {
        SDL_assert(iPlayer != _playerChangeScore);
    }
    _currentScore = eVal;
    _playerChangeScore = iPlayer;
}

void MatchPoints::SetTheWinnerBecauseAbandon(Uint8 playerIx) {
    _playerMatchWin = playerIx;
    _isGameAbandoned = true;
}
