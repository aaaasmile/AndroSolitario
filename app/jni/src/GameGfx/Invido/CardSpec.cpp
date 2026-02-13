#include "CardSpec.h"

#include "CardGfx.h"
#include "InvidoCoreEnv.h"
using namespace invido;

static const std::string g_CardsNameX[] = {
    "Asso di bastoni",    "Due di bastoni",     "Tre di bastoni",
    "Quattro di bastoni", "Cinque di bastoni",  "Sei di bastoni",
    "Sette di bastoni",   "Fante di bastoni",   "Cavallo di bastoni",
    " Re di bastoni",     "Asso di  coppe",     "Due di  coppe",
    "Tre di  coppe",      "Quattro di  coppe",  "Cinque di  coppe",
    "Sei di  coppe",      "Sette di  coppe",    "Fante di  coppe",
    "Cavallo di  coppe",  " Re di  coppe",      "Asso di  denari",
    "Due di  denari",     "Tre di  denari",     "Quattro di  denari",
    "Cinque di  denari",  "Sei di  denari",     "Sette di  denari",
    "Fante di  denari",   "Cavallo di  denari", " Re di  denari",
    "Asso di  spade",     "Due di  spade",      "Tre di  spade",
    "Quattro di  spade",  "Cinque di  spade",   "Sei di  spade",
    "Sette di  spade",    "Fante di  spade",    "Cavallo di  spade",
    " Re di  spade"};

static const int g_PointsTable[] = {
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10,
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10,
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10,
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10};

CardSpec::CardSpec() {
    SetCardIndex(0);
}

void CardSpec::SetCardIndex(Uint8 index) {
    SDL_assert(index >= 0 && index <= eGameConst::NUM_CARDS_MAZZBRI);
    if (index >= 0 && index <= eGameConst::NUM_CARDS_MAZZBRI) {
        _index = index;
    } else {
        return;
    }

    if (_index >= 0 && _index < eGameConst::NUM_CARDS_MAZZBRI) {
        _cardName = std::string(g_CardsNameX[_index]);
    }

    if (_index >= 0 && _index < 10) {
        _eSuit = eSUIT::BASTONI;
    } else if (_index >= 10 && _index < 20) {
        _eSuit = eSUIT::COPPE;
    } else if (_index >= 20 && _index < 30) {
        _eSuit = eSUIT::DENARI;
    } else if (_index >= 30 && _index < 40) {
        _eSuit = eSUIT::SPADE;
    }
}

int CardSpec::GetPoints() {
    SDL_assert(_index >= 0 && _index < eGameConst::NUM_CARDS_MAZZBRI);
    return g_PointsTable[_index];
}
