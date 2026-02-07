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
    eGameLevel GetLevel() { return _eLevel; }
    eTypeOfPLayer GetType() { return _eKind; }
    int GetIndex() { return _index; }
    void SetIndex(int iVal) { _index = iVal; }
    char* GetName() { return _p_PlayerName; }
    void SetName(LPCSTR lpszName);
    I_ALG_Player* GetAlg() { return _p_IAlgorithm; }

    friend std::ostream& operator<<(std::ostream& stream, const Player& o);
    Player(const Player& a);
    Player& operator=(const Player& a);

   private:
    char _p_PlayerName[BUFF_NAME];
    eTypeOfPLayer _eKind;
    int _index;
    I_ALG_Player* _p_IAlgorithm;
    VCT_PALG _vctAlgToDestroy;
    eGameLevel _eLevel;
};

typedef std::vector<Player> VCT_PLAYERS;

// operators
inline std::ostream& operator<<(std::ostream& stream, const Player& o) {
    stream << o._p_PlayerName << std::endl;

    return stream;
}

#endif
