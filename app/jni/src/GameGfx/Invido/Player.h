#ifndef __CPLAYER_H__
#define __CPLAYER_H__

#include <iostream>

#include "AlgCoreInterface.h"
#include "InvidoCoreEnv.h"

namespace invido {

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
    std::string GetName() { return _playerName; }
    void SetName(LPCSTR lpszName);
    I_ALG_Player* GetAlg() { return _p_IAlgorithm; }

    friend std::ostream& operator<<(std::ostream& stream, const Player& o);
    Player(const Player& a);
    Player& operator=(const Player& a);

   private:
    std::string _playerName;
    eTypeOfPLayer _eKind;
    int _index;
    I_ALG_Player* _p_IAlgorithm;
    VCT_PALG _vctAlgToDestroy;
    eGameLevel _eLevel;
};

typedef std::vector<Player> VCT_PLAYERS;

// operators
inline std::ostream& operator<<(std::ostream& stream, const Player& o) {
    stream << o._playerName << std::endl;

    return stream;
}
}

#endif
