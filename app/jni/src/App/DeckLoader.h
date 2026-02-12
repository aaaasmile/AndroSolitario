#ifndef _DECK_LOADER__H_
#define _DECK_LOADER__H_

#include "DeckType.h"
#include "Traits.h"

using namespace traits;

class DeckLoader {
   public:
    DeckLoader();
    ~DeckLoader();

    LPErrInApp LoadCardPac(DeckType& deckType);
    LPErrInApp LoadSymbolsForPac(DeckType& deckType);
    int GetCardWidth() { return _CardWidth; }
    int GetCardHeight() { return _CardHeight; }
    int GetSymbolWidth() { return _SymbolWidth; }
    int GetSymbolHeight() { return _SymbolHeight; }
    SDL_Surface* GetDeckSurface() { return _p_Deck; }
    SDL_Surface* GetSymbolSurface() { return _p_Symbols; }

   private:
    LPErrInApp loadCardPac(SDL_Surface** pp_Deck, DeckType& deckType,
                           Uint16* pac_w, Uint16* pac_h);

   private:
    SDL_Surface* _p_Deck;
    SDL_Surface* _p_Symbols;

    int _CardWidth;
    int _CardHeight;
    int _SymbolWidth;
    int _SymbolHeight;
};

#endif