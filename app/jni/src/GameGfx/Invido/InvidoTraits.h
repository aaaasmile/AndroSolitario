#ifndef INVIDO_TRAITS___H_
#define INVIDO_TRAITS___H_

#include "InvidoCoreDef.h"

namespace invido {

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

// trait Mano
typedef struct {
    void (*const Mano_End)(void* self);
    void (*const NtyWaitingPlayer_Toplay)(void* self, Uint8 playerIx);
    void (*const NtyWaitingPlayer_ToResp)(void* self, Uint8 playerIx);
    void (*const NtyPlayerSayBuiada)(void* self, Uint8 playerIx);
    void (*const ChangeGiocataScore)(void* self, eGiocataScoreState eNewScore);
    void (*const Player_VaVia)(void* self, Uint8 playerIx);
    void (*const Giocata_AMonte)(void* self);
} VManoCb, *LPVManoCb;

typedef struct {
    VManoCb const* tc;
    void* self;
} ManoCb, *LPManoCb;

}  // namespace invido

#endif