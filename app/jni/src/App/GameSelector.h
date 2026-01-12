#ifndef __GAMESELECTOR__H_
#define __GAMESELECTOR__H_

#include "Traits.h"

using namespace traits;

class SolitarioGfx;

class GameSelector{
public:
    GameSelector();
    ~GameSelector();
    GameGfxCb PrepGameGfx();
    

private:
    SolitarioGfx* _p_SolitarioGfx;
};

#endif