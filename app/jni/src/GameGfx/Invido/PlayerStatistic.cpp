#include "PlayerStatistic.h"

#include <SDL3/SDL.h>

using namespace invido;

PlayerStatistic::PlayerStatistic() { Reset(); }

void PlayerStatistic::Reset() {
    lScore = 0;
    bynPointsHad = 0;
}

void PlayerStatistic::Clone(PlayerStatistic* pNew) {
    SDL_assert(pNew);
    pNew->lScore = lScore;
    pNew->bynPointsHad = bynPointsHad;
}
