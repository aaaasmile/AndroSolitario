// Player.cpp

#include "Player.h"

#include "AlgAdvancedPlayer.h"
#include "AlgDummyPlayer.h"

Player::Player() {
    m_iIndex = NOT_VALID_INDEX;
    m_pIAlgorithm = 0;
    m_eLevel = DUMMY;
}

Player::~Player() {
    for (size_t i = 0; i < m_vctAlgToDestroy.size(); i++) {
        delete m_vctAlgToDestroy[i];
    }
}

Player::Player(const Player& a) {
    strncpy(m_pPlayerName, a.m_pPlayerName, BUFF_NAME);
    m_eKind = a.m_eKind;
    m_iIndex = a.m_iIndex;
    m_pIAlgorithm = a.m_pIAlgorithm;
    m_eLevel = a.m_eLevel;
}

Player& Player::operator=(const Player& a) {
    strncpy(m_pPlayerName, a.m_pPlayerName, BUFF_NAME);
    m_eKind = a.m_eKind;
    m_iIndex = a.m_iIndex;
    m_pIAlgorithm = a.m_pIAlgorithm;
    m_eLevel = a.m_eLevel;

    return *this;
}

void Player::Create() {
    CardSpec emptyCard;
    m_eKind = PT_LOCAL;
    strcpy(m_pPlayerName, "Remigiu");

    m_iIndex = NOT_VALID_INDEX;
    m_eLevel = DUMMY;
}

void Player::SetName(LPCSTR lpszName) {
    strncpy(m_pPlayerName, lpszName, BUFF_NAME - 1);
}

void Player::SetType(eTypeOfPLayer eVal) { m_eKind = eVal; }

void Player::SetLevel(eGameLevel eNewLevel, I_ALG_Player* I_val) {
    if (m_pIAlgorithm != 0 && m_eLevel != HMI && m_eLevel != SERVER_LEVEL) {
        delete m_pIAlgorithm;
        m_pIAlgorithm = 0;
    }
    m_eLevel = eNewLevel;

    switch (eNewLevel) {
        case BEGINNER:
            SDL_assert(0);
            break;

        case ADVANCED:
            m_pIAlgorithm = new AlgAdvancedPlayer();
            m_vctAlgToDestroy.push_back(m_pIAlgorithm);
            break;

        case NIGHTMARE:
            SDL_assert(0);
            break;

        case SERVER_LEVEL:
        case HMI:
            SDL_assert(I_val);
            m_pIAlgorithm = I_val;
            break;

        default:
            SDL_assert(0);
    }
}
