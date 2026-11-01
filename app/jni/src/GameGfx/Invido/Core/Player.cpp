#include "Player.h"

#include "AlgPlayer.h"
#include "AlgPlayerAuto.h"
#include "GameActionReplayer.h"

using namespace invido;

Player::Player() {
    _index = NOT_VALID_INDEX;
    _p_IAlgorithm = NULL;
    _eLevel = DUMMY;
}

Player::~Player() { CleanUp(); }

void Player::CleanUp() {
    _vctAlgToDestroy.clear();
    _p_IAlgorithm = NULL;
}

Player::Player(const Player& a) {
    _playerName = a._playerName;
    _eKind = a._eKind;
    _index = a._index;
    _p_IAlgorithm = a._p_IAlgorithm;
    _vctAlgToDestroy = a._vctAlgToDestroy;
    _eLevel = a._eLevel;
}

Player& Player::operator=(const Player& a) {
    if (this == &a) {
        return *this;
    }

    _playerName = a._playerName;
    _eKind = a._eKind;
    _index = a._index;
    _p_IAlgorithm = a._p_IAlgorithm;
    _vctAlgToDestroy = a._vctAlgToDestroy;
    _eLevel = a._eLevel;

    return *this;
}

LPErrInApp Player::InitFromActionPlayer(GameActionReplayer& actionReplayer,
                                        Uint8 playerIx) {
    CleanUp();
    GameActionReplayer::PlayerInfo playerInfo;
    LPErrInApp error = actionReplayer.GetPlayerInfo(playerIx, playerInfo);
    if (error != NULL) {
        return error;
    }

    std::shared_ptr<I_ALG_Player> algorithm(
        new AlgPlayerAuto(actionReplayer, playerIx));
    _p_IAlgorithm = algorithm.get();
    _vctAlgToDestroy.push_back(algorithm);
    _eKind = PT_AUTO;
    _playerName = playerInfo.name;
    _index = playerIx;
    _eLevel = eGameLevel::STANDARD;
    return NULL;
}

void Player::Init(I_ALG_Player* I_val, eTypeOfPLayer eVal, LPCSTR lpszName,
                  Uint8 index) {
    CleanUp();
    if (I_val != NULL) {
        _p_IAlgorithm = I_val;
    } else {
        std::shared_ptr<I_ALG_Player> algorithm(new AlgPlayer());
        _p_IAlgorithm = algorithm.get();
        _vctAlgToDestroy.push_back(algorithm);
    }

    _eKind = eVal;
    _playerName = std::string(lpszName);
    _index = index;
    _eLevel = eGameLevel::STANDARD;
}