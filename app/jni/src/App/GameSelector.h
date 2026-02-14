#ifndef __GAMESELECTOR__H_
#define __GAMESELECTOR__H_

#include "Traits.h"
#include "GameGfx/Invido/InvidoGfx.h"

using namespace traits;

class SolitarioGfx;

class GameSelector {
   public:
    GameSelector();
    ~GameSelector();
    GameGfxCb PrepGameGfx();
    GameHelpPagesCb PrepareGameHelpPages();

   private:
    SolitarioGfx* _p_SolitarioGfx;
    invido::InvidoGfx* _p_InvidoGfx;
};

#endif