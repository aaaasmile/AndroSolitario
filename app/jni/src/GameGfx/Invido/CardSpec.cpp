#include "CardSpec.h"

#include "CardGfx.h"
#include "InvidoCoreEnv.h"

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

CardSpec::CardSpec() { Reset(); }

void CardSpec::SetCardIndex(int itmpIndex) {
    SDL_assert(itmpIndex >= 0 && itmpIndex <= eGameConst::NUM_CARDS_MAZZBRI);
    if (itmpIndex >= 0 && itmpIndex <= eGameConst::NUM_CARDS_MAZZBRI) {
        _cardInfo.byIndex = itmpIndex;
    } else {
        return;
    }

    if (itmpIndex >= 0 && itmpIndex < eGameConst::NUM_CARDS_MAZZBRI) {
        _cardInfo.CardName = g_CardsNameX[itmpIndex];
    }

    if (_cardInfo.byIndex >= 0 && _cardInfo.byIndex < 10) {
        _cardInfo.eSuit = eSUIT::BASTONI;
    } else if (_cardInfo.byIndex >= 10 && _cardInfo.byIndex < 20) {
        _cardInfo.eSuit = eSUIT::COPPE;
    } else if (_cardInfo.byIndex >= 20 && _cardInfo.byIndex < 30) {
        _cardInfo.eSuit = eSUIT::DENARI;
    } else if (_cardInfo.byIndex >= 30 && _cardInfo.byIndex < 40) {
        _cardInfo.eSuit = eSUIT::SPADE;
    }
}

void CardSpec::operator=(const CardSpec& r) {
    _cardInfo.CardName = r._cardInfo.CardName;
    _cardInfo.byIndex = r._cardInfo.byIndex;
    _cardInfo.eSuit = r._cardInfo.eSuit;
}

void CardSpec::Reset() {
    _cardInfo.byIndex = eGameConst::NOT_VALID_INDEX;
    _cardInfo.CardName = "";
    _cardInfo.eSuit = eSUIT::BASTONI;
}

void CardSpec::SetCardInfo(const CARDINFO& Card) {
    _cardInfo.CardName = Card.CardName;
    _cardInfo.byIndex = Card.byIndex;
    _cardInfo.eSuit = Card.eSuit;
}

void CardSpec::FillInfo(CARDINFO* pCardInfo) {
    SDL_assert(pCardInfo);
    pCardInfo->CardName = _cardInfo.CardName;
    pCardInfo->byIndex = _cardInfo.byIndex;
    pCardInfo->eSuit = _cardInfo.eSuit;
}

int CardSpec::GetPoints() {
    int ix = GetCardIndex();
    SDL_assert(ix >= 0 && ix < 40);
    return g_PointsTable[ix];
}
