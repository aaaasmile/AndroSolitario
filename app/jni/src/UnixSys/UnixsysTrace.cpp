#include <SDL3/SDL.h>

void TraceInServiceINFO(char* myBuff) {
    SDL_Log("[INFO] %s", myBuff);
}

void TraceInServiceDEBUG(char* myBuff) {
    SDL_Log("[DBG] %s", myBuff);
}