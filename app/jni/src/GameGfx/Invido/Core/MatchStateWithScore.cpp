#include "MatchStateWithScore.h"

#include "Mano.h"

using namespace invido;

MatchStateWithScore::MatchStateWithScore() {
    _numPlayers = NUM_PLAY_INVIDO_2;
    _playerChangeScore = NOT_VALID_INDEX;
    _isGameAbandoned = false;
    _isMatchEnd = false;
   _currentScore = SC_CANELA;
}

MatchStateWithScore::~MatchStateWithScore() {}

void MatchStateWithScore::MatchStart(int iNumPlayer) {
    _vctPlayerPoints.assign(iNumPlayer, 0);

    _numPlayers = iNumPlayer;
    _playerMatchWin = NOT_VALID_INDEX;
    _scoreGoal = SCORE_GOAL;
    _isMatchInSpecialScore = false;
    _isGameAbandoned = false;
    _isMatchEnd = false;
    _vctGiocataInfo.clear();
}

void MatchStateWithScore::GiocataStart() {
    _vctHandWons.assign(_numPlayers, 0);
    _vctCardPlayed.assign(_numPlayers, CardPlayed{});
    _vctManoInfo.assign(NUM_CARDS_HAND,
                        ManoInfo{NOT_VALID_INDEX, false, false});
    _isMatchEnd = false;
    _isMatchEnd = false;
    _numCardsPlayed = 0;
    _manoRound = 0;
    _eIsGiocataEnd = GES_ON_GOING;
    _currentScore = SC_CANELA;
    _playerWonsHand = NOT_VALID_INDEX;
    _playerFirstHand = NOT_VALID_INDEX;
    _playerGiocataWin = NOT_VALID_INDEX;
    _playerChangeScore = NOT_VALID_INDEX;

    _isGameAbandoned = false;
    _isGiocataAMonte = false;
    _isGiocataPatada = false;
    _isOldManoPatada = false;
    _isManoPatatda = false;
}

void MatchStateWithScore::PlayerPlay(Uint8 playerIx, const CardSpec& cardSpec) {
    SDL_assert(_numCardsPlayed < _vctCardPlayed.size());
    _vctCardPlayed[_numCardsPlayed]._playerIx = playerIx;
    _vctCardPlayed[_numCardsPlayed]._cardSpec = cardSpec;
    _numCardsPlayed++;
}

void MatchStateWithScore::VaDentro(Uint8 playerIx) {
    CardSpec cardSpec;
    cardSpec.SetCardIndex(4);  // make sure that the card will always loose
    SDL_assert(_numCardsPlayed < _vctCardPlayed.size());

    _vctCardPlayed[_numCardsPlayed]._playerIx = playerIx;
    _vctCardPlayed[_numCardsPlayed]._cardSpec.SetCardIndex(
        cardSpec.GetCardIndex());
    _numCardsPlayed++;
}

void MatchStateWithScore::ManoEnd() {
    SDL_assert(_numCardsPlayed == _numPlayers);
    int manoTerminatedIndex = _manoRound;
    SDL_assert(manoTerminatedIndex >= 0 &&
               manoTerminatedIndex < _vctManoInfo.size());
    _manoRound++;
    // hand is terminated
    if (_numPlayers == NUM_PLAY_INVIDO_2) {
        int points_1 = _vctCardPlayed[PLAYER1]._cardSpec.GetPoints();
        int player_1 = _vctCardPlayed[PLAYER1]._playerIx;
        int points_2 = _vctCardPlayed[PLAYER2]._cardSpec.GetPoints();
        int player_2 = _vctCardPlayed[PLAYER2]._playerIx;

        _isManoPatatda = false;
        // mano is played
        _vctManoInfo[manoTerminatedIndex]._isPlayed = true;

        if (points_1 == points_2) {
            // nobody wins the hand
            _isManoPatatda = true;
            if (_playerFirstHand != NOT_VALID_INDEX) {
                // the first player who take the mano is the giocata winner
                _playerGiocataWin = _playerFirstHand;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = NOT_VALID_INDEX;
            _vctManoInfo[manoTerminatedIndex]._isPata = true;

            // mark the mano patada, the next who take the trick win
            _isOldManoPatada = true;
        } else if (points_1 > points_2) {
            // first player wons the mano
            _vctHandWons[player_1]++;
            if (_playerFirstHand == NOT_VALID_INDEX) {
                _playerFirstHand = player_1;
            }
            if (_vctHandWons[player_1] >= NUM_PLAY_INVIDO_2 ||
                _isOldManoPatada) {
                // giocata is terminated, giocata winner is the first player in
                // this hand
                _playerGiocataWin = player_1;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = player_1;
            _vctManoInfo[manoTerminatedIndex]._playerIndex = player_1;
        } else {
            // second player catch the mano
            _vctHandWons[player_2]++;
            if (_playerFirstHand == NOT_VALID_INDEX) {
                _playerFirstHand = player_2;
            }
            if (_vctHandWons[player_2] >= NUM_PLAY_INVIDO_2 ||
                _isOldManoPatada) {
                // giocata is terminated, giocata winner is the second player in
                // this hand
                _playerGiocataWin = player_2;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = player_2;
            _vctManoInfo[manoTerminatedIndex]._playerIndex = player_2;
        }
        if (_manoRound >= NUM_CARDS_HAND) {
            // giocata is terminated
            if (_isManoPatatda && (_playerFirstHand == NOT_VALID_INDEX)) {
                // strange case all hands was patada. Giocata is also patada.
                _eIsGiocataEnd = GES_PATADA;
                _playerGiocataWin = NOT_VALID_INDEX;
                GiocataPatada();
            } else {
                // giocata winner must be already defined
                SDL_assert(_playerGiocataWin != NOT_VALID_INDEX);
                GiocataEndWithWinner(_playerGiocataWin);
            }
        }
    } else {
        SDL_assert(0);
    }
    _numCardsPlayed = 0;
}

void MatchStateWithScore::PlayerVaVia(Uint8) {
    _eIsGiocataEnd = GES_HAVE_WINNER;
}

void MatchStateWithScore::GiocataPatada() {
    GiocataInfo gi = {._score = SC_PATTA};
    _vctGiocataInfo.push_back(gi);
    _vctManoInfo.clear();
    _isGiocataPatada = true;
}

void MatchStateWithScore::GiocataAMonte() {
    _eIsGiocataEnd = GES_AMONTE;
    GiocataInfo gi = {._score = SC_AMONTE};
    _vctGiocataInfo.push_back(gi);
    _vctManoInfo.clear();
    _isGiocataAMonte = true;
}

void MatchStateWithScore::GiocataEndWithWinner(Uint8 playerWinIx) {
    SDL_assert(_eIsGiocataEnd == GES_HAVE_WINNER);
    SDL_assert(playerWinIx < _vctPlayerPoints.size());
    _playerGiocataWin = playerWinIx;
    _vctPlayerPoints[_playerGiocataWin] += _currentScore;
    GiocataInfo gi = {._playerIndex = _playerGiocataWin,
                      ._score = _currentScore};
    _vctGiocataInfo.push_back(gi);

    if (_vctPlayerPoints[_playerGiocataWin] >= _scoreGoal) {
        // match is terminated
        _playerMatchWin = _playerGiocataWin;
        _isMatchEnd = true;
    } else if (_vctPlayerPoints[PLAYER1] == SPECIAL_SCORE &&
               _vctPlayerPoints[PLAYER2] == SPECIAL_SCORE) {
        // special condition both player are on 23 to 23 or after 7 to 7....
        beginSpecialTurn();
    } else if (_isMatchInSpecialScore &&
               (_vctPlayerPoints[PLAYER1] == SCORE_SEVEN &&
                _vctPlayerPoints[PLAYER2] == SCORE_SEVEN)) {
        beginSpecialTurn();
    }

    _vctManoInfo.clear();
}

void MatchStateWithScore::beginSpecialTurn() {
    _scoreGoal = SPECIAL_SCORE_GOAL;
    _vctPlayerPoints[PLAYER1] = 0;
    _vctPlayerPoints[PLAYER2] = 0;
    _isMatchInSpecialScore = true;
}

void MatchStateWithScore::GetManoInfo(int manoNum, Uint8* pplayerIx, bool* pisPlayed,
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

void MatchStateWithScore::GetGiocataInfo(int iNumGiocata, GiocataInfo* pGiocInfo) {
    SDL_assert(pGiocInfo);
    if (iNumGiocata >= 0 && iNumGiocata < (int)_vctGiocataInfo.size()) {
        *pGiocInfo = _vctGiocataInfo[iNumGiocata];
    }
}

void MatchStateWithScore::ChangeCurrentScore(eGiocataScoreState eVal, int iPlayer) {
    if (eVal > 0) {
        SDL_assert(iPlayer != _playerChangeScore);
    }
    _currentScore = eVal;
    _playerChangeScore = iPlayer;
}

void MatchStateWithScore::SetTheWinnerBecauseAbandon(Uint8 playerIx) {
    _playerMatchWin = playerIx;
    _isGameAbandoned = true;
    _isMatchEnd = true;
}
