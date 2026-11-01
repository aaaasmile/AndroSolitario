#ifndef __PLAYERSONTABLE_H
#define __PLAYERSONTABLE_H

#include "Player.h"

namespace invido {
class PlayersOnTable {
   public:
    PlayersOnTable();
    void AddPlayer(Player& player);
    void SetFirstOnMatch(Uint8 index);
    Uint8 GetFirstOnMatch() { return _firstOnMatch; }
    void SetCurrentAndFirstOnTrick(Uint8 index);
    Uint8 GetFirstOnTrick() { return _firstOnTrick; }
    void SetFirstOnGiocata(Uint8 index);
    Uint8 GetFirstOnGiocata() { return _firstOnGiocata; }
    Player* GetPlayerToPlay();
    Player* SwitchToNextPlayer();
    Player* GetPlayerOnIndex(Uint8 index);
    Player* PeekNextPlayerToIx(Uint8 index);
    int CalcDistance(int iPlayerRef, int PlayerTmp);
    void CalcCircleIndex(int* paPlayerDeck, size_t size);
    size_t GetNumOfPlayers() { return _vctPlayers.size(); }

   private:
    VCT_PLAYER _vctPlayers;
    Uint8 _current;
    Uint8 _firstOnGiocata;
    Uint8 _firstOnTrick;
    Uint8 _firstOnMatch;
};

}  // namespace invido

#endif