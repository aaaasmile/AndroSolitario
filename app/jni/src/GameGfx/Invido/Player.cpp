// cPlayer.cpp

#include "cAlgAdvancedPlayer.h"
#include "cAlgDummyPlayer.h"
#include "cPlayer.h"


////////////////////////////////////////////////////////////////
//  *************************  PLAYER CLASS *******************
////////////////////////////////////////////////////////////////

cPlayer::cPlayer() {
    m_iIndex = NOT_VALID_INDEX;
    m_pIAlgorithm = 0;
    m_eLevel = DUMMY;
}

////////////////////////////////////////
//       ~cPlayer
/*! Destructor
 */
cPlayer::~cPlayer() {
    for (size_t i = 0; i < m_vctAlgToDestroy.size(); i++) {
        delete m_vctAlgToDestroy[i];
    }
}

cPlayer::cPlayer(const cPlayer& a) {
    strncpy(m_pPlayerName, a.m_pPlayerName, BUFF_NAME);
    m_eKind = a.m_eKind;
    m_iIndex = a.m_iIndex;
    m_pIAlgorithm = a.m_pIAlgorithm;
    m_eLevel = a.m_eLevel;
}

////////////////////////////////////////
//       operator=
/*! Copy player operator
// \param const cPlayer&a :
*/
cPlayer& cPlayer::operator=(const cPlayer& a) {
    strncpy(m_pPlayerName, a.m_pPlayerName, BUFF_NAME);
    m_eKind = a.m_eKind;
    m_iIndex = a.m_iIndex;
    m_pIAlgorithm = a.m_pIAlgorithm;
    m_eLevel = a.m_eLevel;

    return *this;
}

void cPlayer::Create() {
    CardSpec emptyCard;
    m_eKind = PT_LOCAL;
    strcpy(m_pPlayerName, "Remigiu");

    m_iIndex = NOT_VALID_INDEX;
    m_eLevel = DUMMY;
}

void cPlayer::SetName(LPCSTR lpszName) {
    strncpy(m_pPlayerName, lpszName, BUFF_NAME - 1);
}

void cPlayer::SetType(eTypeOfPLayer eVal) { m_eKind = eVal; }

void cPlayer::SetLevel(eGameLevel eNewLevel, I_ALG_Player* I_val) {
    if (m_pIAlgorithm != 0 && m_eLevel != HMI && m_eLevel != SERVER_LEVEL) {
        delete m_pIAlgorithm;
        m_pIAlgorithm = 0;
    }
    m_eLevel = eNewLevel;

    switch (eNewLevel) {
        case BEGINNER:
            ASSERT(0);
            break;

        case ADVANCED:
            m_pIAlgorithm = new cAlgAdvancedPlayer();
            m_vctAlgToDestroy.push_back(m_pIAlgorithm);
            break;

        case NIGHTMARE:
            ASSERT(0);
            break;

        case SERVER_LEVEL:
        case HMI:
            ASSERT(I_val);
            m_pIAlgorithm = I_val;
            break;

        default:
            ASSERT(0);
    }
}
