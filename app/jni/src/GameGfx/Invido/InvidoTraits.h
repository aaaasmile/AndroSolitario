#ifndef INVIDO_TRAITS___H_
#define INVIDO_TRAITS___H_

#include <SDL3/SDL.h>

// trait Partita

typedef struct {
    void (*const PartitaStarted)(void* self, Uint8 playerIx);
    void (*const PartitaEnded)(void* self); 
} VPartitaCb, *LPVPartitaCb;

typedef struct {
    VPartitaCb const* tc;
    void* self;
} PartitaCb, *LPPartitaCb;

// trait Giocata
typedef struct {
    void (*const GiocataStarted)(void* self, Uint8 playerStartIx);
    void (*const GiocataEndedWinner)(void* self, Uint8 playerWinIx);
    void (*const GiocataEndDueced)(void* self);
} VGiocataCb, *LPVGiocataCb;

typedef struct {
    VGiocataCb const* tc;
    void* self;
} GiocataCb, *LPGiocataCb;


#endif