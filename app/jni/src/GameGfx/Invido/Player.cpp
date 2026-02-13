#include "Player.h"

#include "AlgPlayer.h"

using namespace invido;

Player::Player() {
    _index = NOT_VALID_INDEX;
    _p_IAlgorithm = 0;
    _eLevel = DUMMY;
}

Player::~Player() {
    for (size_t i = 0; i < _vctAlgToDestroy.size(); i++) {
        delete _vctAlgToDestroy[i];
    }
}

Player::Player(const Player& a) {
    strncpy(_p_PlayerName, a._p_PlayerName, BUFF_NAME);
    _eKind = a._eKind;
    _index = a._index;
    _p_IAlgorithm = a._p_IAlgorithm;
    _eLevel = a._eLevel;
}

Player& Player::operator=(const Player& a) {
    strncpy(_p_PlayerName, a._p_PlayerName, BUFF_NAME);
    _eKind = a._eKind;
    _index = a._index;
    _p_IAlgorithm = a._p_IAlgorithm;
    _eLevel = a._eLevel;

    return *this;
}

void Player::Create() {
    CardSpec emptyCard;
    _eKind = PT_LOCAL;
    strcpy(_p_PlayerName, "Remigiu");

    _index = NOT_VALID_INDEX;
    _eLevel = DUMMY;
}

void Player::SetName(LPCSTR lpszName) {
    strncpy(_p_PlayerName, lpszName, BUFF_NAME - 1);
}

void Player::SetType(eTypeOfPLayer eVal) { _eKind = eVal; }

void Player::SetLevel(eGameLevel eNewLevel, I_ALG_Player* I_val) {
    if (_p_IAlgorithm != 0 && _eLevel != HMI && _eLevel != SERVER_LEVEL) {
        delete _p_IAlgorithm;
        _p_IAlgorithm = 0;
    }
    _eLevel = eNewLevel;

    switch (eNewLevel) {
        case BEGINNER:
            SDL_assert(0);
            break;

        case ADVANCED:
            _p_IAlgorithm = new AlgAdvancedPlayer();
            _vctAlgToDestroy.push_back(_p_IAlgorithm);
            break;

        case NIGHTMARE:
            SDL_assert(0);
            break;

        case SERVER_LEVEL:
        case HMI:
            SDL_assert(I_val);
            _p_IAlgorithm = I_val;
            break;

        default:
            SDL_assert(0);
    }
}
