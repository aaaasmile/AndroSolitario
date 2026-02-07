#include "MatchPoints.h"

#include "Mano.h"

MatchPoints::MatchPoints() {
    _numPlayers = NUM_PLAY_INVIDO_2;
    _p_Mano = 0;
    _isGameAbandoned = false;
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        _ManoDetailInfo[iManoNum].Reset();
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
    CardSpec CardUndef;
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        _vctHandWons[i] = 0;
        _vctCardPlayed[i].cardSpec = CardUndef;
        _vctCardPlayed[i].playerIx = NOT_VALID_INDEX;
    }
    _numCardsPlayed = 0;
    _currentScore = SC_CANELA;
    _playerWonsHand = NOT_VALID_INDEX;
    _playerFirstHand = NOT_VALID_INDEX;
    _playerGiocataWin = NOT_VALID_INDEX;
    _eIsGiocataEnd = GES_ON_GOING;
    _isManoPatatda = false;
    _manoRound = 0;
    _isOldManoPatada = false;
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        _ManoDetailInfo[iManoNum].Reset();
    }
    _playerChangeScore = NOT_VALID_INDEX;
    _isGameAbandoned = false;
}

void MatchPoints::PlayerPlay(int iPlayerIx, CARDINFO* pCard) {
    CardSpec Card;
    SDL_assert(pCard);
    Card.SetCardInfo(*pCard);
    SDL_assert(_numCardsPlayed < MAX_NUM_PLAYER && _numCardsPlayed >= 0);
    _vctCardPlayed[_numCardsPlayed].playerIx = iPlayerIx;
    _vctCardPlayed[_numCardsPlayed].cardSpec = Card;
    _numCardsPlayed++;
}

void MatchPoints::ManoEnd() {
    SDL_assert(_numCardsPlayed == _numPlayers);
    int iManoTerminatedIndex = _manoRound;
    SDL_assert(iManoTerminatedIndex >= 0 &&
               iManoTerminatedIndex < NUM_CARDS_HAND);
    _manoRound++;
    // hand is terminated
    if (_numPlayers == NUM_PLAY_INVIDO_2)  // consistency check
    {
        int iPoints_1 =
            g_PointsTable[_vctCardPlayed[PLAYER1].cardSpec.GetCardIndex()];
        int iPlayer_1 = _vctCardPlayed[PLAYER1].playerIx;
        int iPoints_2 =
            g_PointsTable[_vctCardPlayed[PLAYER2].cardSpec.GetCardIndex()];
        int iPlayer_2 = _vctCardPlayed[PLAYER2].playerIx;
        _isManoPatatda = false;
        // mano is played
        _ManoDetailInfo[iManoTerminatedIndex].isPlayed = true;

        if (iPoints_1 == iPoints_2) {
            // nobody wins the hand
            _isManoPatatda = true;
            if (_playerFirstHand != NOT_VALID_INDEX) {
                // the first player who take the mano is the giocata winner
                _playerGiocataWin = _playerFirstHand;
                _eIsGiocataEnd = GES_HAVE_WINNER;
            }
            _playerWonsHand = NOT_VALID_INDEX;
            _ManoDetailInfo[iManoTerminatedIndex].isPata = true;

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
            _ManoDetailInfo[iManoTerminatedIndex].playerIndex = iPlayer_1;
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
            _ManoDetailInfo[iManoTerminatedIndex].playerIndex = iPlayer_2;
        }
        if (_manoRound >= NUM_CARDS_HAND) {
            // giocata is terminated
            if (_isManoPatatda && (_playerFirstHand == NOT_VALID_INDEX)) {
                // strange case all hands was patadi. Giocata is also patada.
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

void MatchPoints::PlayerVaVia(int iPlayerIx) {
    _eIsGiocataEnd = GES_HAVE_WINNER;
    if (iPlayerIx == PLAYER1) {
        _playerGiocataWin = PLAYER2;
    } else {
        _playerGiocataWin = PLAYER1;
    }
}

void MatchPoints::GiocataEnd() {
    if (_eIsGiocataEnd == GES_HAVE_WINNER) {
        SDL_assert(_playerGiocataWin != NOT_VALID_INDEX);
        // update the score
        _vctPlayerPoints[_playerGiocataWin] += _currentScore;
        _vctGiocataInfo.push_back(
            GiocataInfo(_playerGiocataWin, _currentScore));

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
    } else {
        // pata or monte
        _vctGiocataInfo.push_back(GiocataInfo(NOT_VALID_INDEX, SC_AMONTE));
    }
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        _ManoDetailInfo[iManoNum].Reset();
    }
}

bool MatchPoints::IsGiocatEnd() {
    bool bRet = false;

    if (_eIsGiocataEnd == GES_AMONTE || _eIsGiocataEnd == GES_PATADA ||
        _eIsGiocataEnd == GES_HAVE_WINNER) {
        bRet = true;
    }
    return bRet;
}

void MatchPoints::AMonte() { _eIsGiocataEnd = GES_AMONTE; }

void MatchPoints::beginSpecialTurn() {
    _scoreGoal = SPECIAL_SCORE_GOAL;
    _vctPlayerPoints[PLAYER1] = 0;
    _vctPlayerPoints[PLAYER2] = 0;
    _isMatchInSpecialScore = true;
}

void MatchPoints::GetManoInfo(int iManoNum, int* piPlayerIx, bool* pbIsPlayed,
                              bool* pbIsPata) {
    SDL_assert(pbIsPata);
    SDL_assert(pbIsPlayed);
    SDL_assert(piPlayerIx);
    if (iManoNum >= 0 && iManoNum < NUM_CARDS_HAND) {
        *pbIsPata = _ManoDetailInfo[iManoNum].isPata;
        *pbIsPlayed = _ManoDetailInfo[iManoNum].isPlayed;
        *piPlayerIx = _ManoDetailInfo[iManoNum].playerIndex;

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

void MatchPoints::SetTheWinner(int iPlayerIx) {
    _playerMatchWin = iPlayerIx;
    _isGameAbandoned = true;
}
