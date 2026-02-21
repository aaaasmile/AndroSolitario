#ifndef __CPLAYER_H__
#define __CPLAYER_H__

#include <iostream>

#include "AlgCoreInterface.h"
#include "InvidoCoreDef.h"

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
    void Init(I_ALG_Player* I_val, eTypeOfPLayer eVal, LPCSTR lpszName,
                Uint8 index);
    void SetName(LPCSTR lpszName) { _playerName = std::string(lpszName); }
    std::string GetName() { return _playerName; }
    void SetType(eTypeOfPLayer eVal) { _eKind = eVal; }
    eTypeOfPLayer GetType() { return _eKind; }
    void SetLevel(eGameLevel eNewLevel) { _eLevel = eNewLevel; }
    eGameLevel GetLevel() { return _eLevel; }
    Uint8 GetIndex() { return _index; }
    void SetIndex(Uint8 iVal) { _index = iVal; }

    I_ALG_Player* GetAlg() { return _p_IAlgorithm; }

    friend std::ostream& operator<<(std::ostream& stream, const Player& o);
    Player(const Player& a);
    Player& operator=(const Player& a);

   private:
    std::string _playerName;
    eTypeOfPLayer _eKind;
    Uint8 _index;
    I_ALG_Player* _p_IAlgorithm;
    VCT_PALG _vctAlgToDestroy;
    eGameLevel _eLevel;
};

typedef std::vector<Player> VCT_PLAYER;

// operators
inline std::ostream& operator<<(std::ostream& stream, const Player& o) {
    stream << o._playerName << std::endl;

    return stream;
}
}  // namespace invido

#endif
