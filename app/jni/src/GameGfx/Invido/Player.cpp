#include "Player.h"

#include "AlgPlayer.h"

using namespace invido;

Player::Player() {
    _index = NOT_VALID_INDEX;
    _p_IAlgorithm = NULL;
    _eLevel = DUMMY;
}

Player::~Player() {
    for (size_t i = 0; i < _vctAlgToDestroy.size(); i++) {
        delete _vctAlgToDestroy[i];
    }
}

Player::Player(const Player& a) {
    _playerName = a._playerName;
    _eKind = a._eKind;
    _index = a._index;
    _p_IAlgorithm = a._p_IAlgorithm;
    _eLevel = a._eLevel;
}

Player& Player::operator=(const Player& a) {
    _playerName = a._playerName;
    _eKind = a._eKind;
    _index = a._index;
    _p_IAlgorithm = a._p_IAlgorithm;
    _eLevel = a._eLevel;

    return *this;
}

void Player::Init(I_ALG_Player* I_val, eTypeOfPLayer eVal, LPCSTR lpszName,
                    Uint8 index) {
    if (I_val != NULL) {
        _p_IAlgorithm = I_val;
    } else {
        _p_IAlgorithm = new AlgPlayer();
        _vctAlgToDestroy.push_back(_p_IAlgorithm);
    }

    _eKind = eVal;
    _playerName = std::string(lpszName);
    _index = index;
    _eLevel = eGameLevel::STANDARD;
}