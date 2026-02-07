

// Player.h

#ifndef __CPLAYER_H__
#define __CPLAYER_H__

#include "CardSpec.h"
#include "InvidoCoreEnv.h"
#include "PlayerStatistic.h"

enum eCARDINDEX { CIX_0 = 0, CIX_1 = 1, CIX_2 = 2 };

inline eCARDINDEX& operator++(eCARDINDEX& eF) {
    return eF = (eF >= CIX_2) ? CIX_0 : eCARDINDEX(eF + 1);
}

inline eCARDINDEX& operator++(eCARDINDEX& eF, int) {
    return eF = (eF >= CIX_2) ? CIX_0 : eCARDINDEX(eF + 1);
}

class Player {
    typedef std::vector<I_ALG_Player*> VCT_PALG;

   public:
    Player();
    virtual ~Player();
    void Create();
    void SetType(eTypeOfPLayer eVal);
    void SetLevel(eGameLevel eNewLevel, I_ALG_Player* I_val);
    eGameLevel GetLevel() { return m_eLevel; }
    eTypeOfPLayer GetType() { return m_eKind; }
    int GetIndex() { return m_iIndex; }
    void SetIndex(int iVal) { m_iIndex = iVal; }
    char* GetName() { return m_pPlayerName; }
    void SetName(LPCSTR lpszName);
    I_ALG_Player* GetAlg() { return m_pIAlgorithm; }

    friend std::ostream& operator<<(std::ostream& stream, const Player& o);
    Player(const Player& a);
    Player& operator=(const Player& a);

   private:
    char m_pPlayerName[BUFF_NAME];
    eTypeOfPLayer m_eKind;
    int m_iIndex;
    I_ALG_Player* m_pIAlgorithm;
    VCT_PALG m_vctAlgToDestroy;
    eGameLevel m_eLevel;
};

typedef std::vector<Player> VCT_PLAYERS;

// operators
inline std::ostream& operator<<(std::ostream& stream, const Player& o) {
    stream << o.m_pPlayerName << std::endl;

    return stream;
}

#endif
