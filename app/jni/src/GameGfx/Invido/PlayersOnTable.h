#ifndef __PLAYERSONTABLE_H
#define __PLAYERSONTABLE_H

#include "Player.h"
class PlayersOnTable {
   public:
    enum eSwitchPLayer { SWITCH_TO_NEXT, NO_SWITCH };
    PlayersOnTable();
    void SetFirstOnMatch(long lIndex);
    long GetFirstOnMatch() { return m_lFirstOnMatch; }
    void SetFirstOnTrick(long lIndex);
    long GetFirstOnTrick() { return m_lFirstOnTrick; }
    void SetFirstOnGiocata(long lIndex);
    long GetFirstOnGiocata() { return m_lFirstOnGiocata; }
    void Create(Player* pHmiPlayer, int iNumPlayers);
    Player* GetPlayerToPlay(eSwitchPLayer eVal);
    Player* GetPlayerIndex(long lIndex);
    int CalcDistance(int iPlayerRef, int PlayerTmp);
    void CalcCircleIndex(int* paPlayerDeck);
    void CalcCircleIndex_Cust(int* paPlayerDeck, int iPlayerIni);
    bool IsLevelPython();

   private:
    VCT_PLAYERS m_vctPlayers;
    long m_lCurrent;
    long m_lNumPlayers;
    long m_lFirstOnGiocata;
    long m_lFirstOnTrick;
    long m_lFirstOnMatch;
};

#endif