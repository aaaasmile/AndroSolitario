#ifndef INVIDO_TRAITS___H_
#define INVIDO_TRAITS___H_

#include <SDL3/SDL.h>

// trait Partita

typedef struct {
    void (*const PartitaEnd)(void* self);
    void (*const NewGiocata)(void* self, Uint8 playerStartIx);
} VPartitaCb, *LPVPartitaCb;

typedef struct {
    VPartitaCb const* tc;
    void* self;
} PartitaCb, *LPPartitaCb;

#endif