#include <SDL3/SDL.h>
#include <inttypes.h>

void TraceInServiceINFO(char* myBuff) {
    SDL_Log("[%" PRIu64 "- DBG] %s", SDL_GetTicks(), myBuff);
}

void TraceInServiceDEBUG(char* myBuff) {
    SDL_Log("[%" PRIu64 "- DBG] %s", SDL_GetTicks(), myBuff);
}