#include "CardGfx.h"

static STRING g_name[] = {
    "Asso",    "Due",     "Tre",     "Quattro", "Cinque",  "Sei",     "Sette",
    "Fante",   "Cavallo", "Re",      "Asso",    "Due",     "Tre",     "Quattro",
    "Cinque",  "Sei",     "Sette",   "Fante",   "Cavallo", "Re",      "Asso",
    "Due",     "Tre",     "Quattro", "Cinque",  "Sei",     "Sette",   "Fante",
    "Cavallo", "Re",      "Asso",    "Due",     "Tre",     "Quattro", "Cinque",
    "Sei",     "Sette",   "Fante",   "Cavallo", "Re"};
static STRING g_nameTarock[] = {
    "Asso", "Due",  "Tre",   "Quattro", "Cinque", "Sei",   "Sette",
    "Otto", "Nove", "Dieci", "Fante",   "Cav",    "Donna", "Re",
    "Asso", "Due",  "Tre",   "Quattro", "Cinque", "Sei",   "Sette",
    "Otto", "Nove", "Dieci", "Fante",   "Cav",    "Donna", "Re",
    "Asso", "Due",  "Tre",   "Quattro", "Cinque", "Sei",   "Sette",
    "Otto", "Nove", "Dieci", "Fante",   "Cav",    "Donna", "Re",
    "Asso", "Due",  "Tre",   "Quattro", "Cinque", "Sei",   "Sette",
    "Otto", "Nove", "Dieci", "Fante",   "Cav",    "Donna", "Re"};

CardGfx::CardGfx() {
    _idx = 0;
    _faceUp = true;
    _rank = 0;
    _eSuit = eSUIT::BASTONI;
    _x = _y = _width = _height = 0;
    _pPacDeck = 0;
}

LPCSTR CardGfx::String() {
    STRING res;
    switch (_eSuit) {
        case eSUIT::BASTONI:
            res = _name + " Bastoni";
            break;
        case eSUIT::COPPE:
            res = _name + " Coppe";
            break;
        case eSUIT::DENARI:
            res = _name + " Denari";
            break;
        case eSUIT::SPADE:
            res = _name + " Spade";
            break;
        default:
            break;
    }
    char dest[256];
    snprintf(dest, sizeof(dest), "%s-ix(%d)-rank(%d)-faceUP(%s)", res.c_str(),
             _idx, _rank, _faceUp ? "yes" : "no");
    _nameFull = dest;
    return _nameFull.c_str();
}

LPErrInApp CardGfx::SetIdx(int ix, DeckType& deckType) {
    _idx = ix;
    _deckType = deckType;
    _rank = _deckType.GetRank(ix);
    if (deckType.GetType() == eDeckType::TAROCK_PIEMONT) {
        _name = g_nameTarock[_idx];
        if (_idx >= 0 && _idx <= 13)
            _eSuit = BASTONI;
        else if (_idx > 13 && _idx <= 27)
            _eSuit = COPPE;
        else if (_idx > 27 && _idx <= 41)
            _eSuit = DENARI;
        else if (_idx > 41 && _idx <= 55)
            _eSuit = SPADE;
        else
            return ERR_UTIL::ErrorCreate("Error SetIdx %d is out of range", ix);
    } else {
        _name = g_name[_idx];
        if (_idx >= 0 && _idx <= 9)
            _eSuit = BASTONI;
        else if (_idx > 9 && _idx <= 19)
            _eSuit = COPPE;
        else if (_idx > 19 && _idx <= 29)
            _eSuit = DENARI;
        else if (_idx > 29 && _idx <= 39)
            _eSuit = SPADE;
        else
            return ERR_UTIL::ErrorCreate("Error SetIdx %d is out of range", ix);
    }

    return NULL;
}

const char* CardGfx::SuitStr() {
    switch (_eSuit) {
        case BASTONI:
            return "Bastoni";
        case COPPE:
            return "Coppe";
        case DENARI:
            return "Denari";
        case SPADE:
            return "Spade";
        default:
            break;
    }
    return "";
}

LPErrInApp CardGfx::DrawCardPac(SDL_Surface* s) {
    int index = Index();
    int num_cards_insuit = _deckType.GetNumCardInSuit();
    SDL_assert(_width != 0);
    SDL_assert(_height != 0);

    if (index < 0 || index > _deckType.GetNumCards())
        return ERR_UTIL::ErrorCreate("Card index outside the range %d", index);
    if (_width == 0 || _height == 0) {
        return ERR_UTIL::ErrorCreate(
            "CardGfx - DrawCardPac size is not defined");
    }

    int suitIx = index / num_cards_insuit;
    int cardIx = index % num_cards_insuit;
    SDL_Rect srcCard;

    srcCard.x = suitIx * _width;
    srcCard.y = cardIx * _height;
    srcCard.w = _width;
    srcCard.h = _height;

    SDL_Rect dest;
    dest.x = X();
    dest.y = Y();

    if (!SDL_BlitSurface(_pPacDeck, &srcCard, s, &dest)) {
        return ERR_UTIL::ErrorCreate(
            "SDL_BlitSurface in DrawCardPac with Iterator error: %s\n",
            SDL_GetError());
    }
    return NULL;
}