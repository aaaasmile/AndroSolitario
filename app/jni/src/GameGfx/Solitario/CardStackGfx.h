#ifndef CARDSTACKGFX__H
#define CARDSTACKGFX__H

#include <stdlib.h>
#include <time.h>

#include <vector>

#include "CardGfx.h"
#include "DeckType.h"
#include "ErrorInfo.h"

class CardStackGfx {
   public:
    CardStackGfx();
    LPErrInApp NewDeck(DeckType& deckType, int widthEmpty, int heightEmpty);
    void Shuffle();
    void Clear() { _vct_lpCardGfx.clear(); }
    void CleanUp();
    void Sort();
    void Reverse();
    bool IsEmpty() { return _vct_lpCardGfx.empty(); }
    int Size() { return _vct_lpCardGfx.size(); }

    void PushCard(const LPCardGfx pCard);
    void PushStack(CardStackGfx* pCardstack);

    LPCardGfx PopCard();
    CardStackGfx* PopStack(int items);

    LPCardGfx RemoveCard(int index);
    void InsertCard(int index, LPCardGfx pCard);

    int GetCardWherePointIsInside(int x, int y);
    LPCardGfx GetCard(int index);

    void SetCardsFaceUp(bool bVal);
    void SetDeckSurface(SDL_Surface* pSurface);
    void SetScaleFactor(float factor);
    LPCardGfx First() {
        SDL_assert(_vct_lpCardGfx.size() > 0);
        return _vct_lpCardGfx[0];
    }
    LPCardGfx Last() {
        if (_vct_lpCardGfx.empty()) {
            return NULL;
        }
        return _vct_lpCardGfx[_vct_lpCardGfx.size() - 1];
    }
    LPCardGfx Item(int ix) {
        if (ix < 0 || ix >= (int)_vct_lpCardGfx.size()) {
            return NULL;
        }
        return _vct_lpCardGfx[ix];
    }

   private:
    std::vector<LPCardGfx> _vct_lpCardGfx;
    DeckType _deckType;
    int _widthEmpty;
    int _heightEmpty;
    float _scaleFactor;
};

typedef CardStackGfx* LPCardStackGfx;

#endif
