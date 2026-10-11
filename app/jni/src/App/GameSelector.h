#ifndef __GAMESELECTOR__H_
#define __GAMESELECTOR__H_

#include "Languages.h"
#include "Traits.h"

using namespace traits;

class GameSelector {
   public:
    GameSelector();
    ~GameSelector();
    GameGfxCb PrepGameGfx();
    GameHelpPagesCb PrepareGameHelpPages();
    enum Languages::eStringID GetGameTitle();

   private:
    void* _p_Gfx;
};

#endif