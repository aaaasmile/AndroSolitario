#ifndef __GAMESELECTOR__H_
#define __GAMESELECTOR__H_

#include "Traits.h"

using namespace traits;

class SolitarioGfx;
class InvidoGfx;

class GameSelector {
   public:
    GameSelector();
    ~GameSelector();
    GameGfxCb PrepGameGfx();
    GameHelpPagesCb PrepareGameHelpPages();

   private:
    SolitarioGfx* _p_SolitarioGfx;
    InvidoGfx* _p_InvidoGfx;
};

#endif