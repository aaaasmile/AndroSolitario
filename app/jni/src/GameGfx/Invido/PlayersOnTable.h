#ifndef __PLAYERSONTABLE_H
#define __PLAYERSONTABLE_H

#include "Player.h"

namespace invido {
class PlayersOnTable {
   public:
    enum eSwitchPLayer { SWITCH_TO_NEXT, NO_SWITCH };
    PlayersOnTable();
    void SetFirstOnMatch(Uint8 lIndex);
    Uint8 GetFirstOnMatch() { return _firstOnMatch; }
    void SetFirstOnTrick(Uint8 lIndex);
    Uint8 GetFirstOnTrick() { return _firstOnTrick; }
    void SetFirstOnGiocata(Uint8 lIndex);
    Uint8 GetFirstOnGiocata() { return _firstOnGiocata; }
    void Create(Player* pHmiPlayer, int iNumPlayers);
    Player* GetPlayerToPlay(eSwitchPLayer eVal);
    Player* GetPlayerIndex(Uint8 lIndex);
    int CalcDistance(int iPlayerRef, int PlayerTmp);
    void CalcCircleIndex(int* paPlayerDeck);
    void CalcCircleIndex_Cust(int* paPlayerDeck, int iPlayerIni);
    bool IsLevelPython();

   private:
    VCT_PLAYERS _vctPlayers;
    Uint8 _current;
    Uint8 _numPlayers;
    Uint8 _firstOnGiocata;
    Uint8 _firstOnTrick;
    Uint8 _firstOnMatch;
};

}

#endif