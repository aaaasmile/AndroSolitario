#include "CardStackGfx.h"

#include <random>

#include "WinTypeGlobal.h"

LPErrInApp CardStackGfx::NewDeck(DeckType& deckType, int widthEmpty,
                                 int heightEmpty) {
    LPErrInApp err;
    _deckType = deckType;
    _widthEmpty = widthEmpty;
    _heightEmpty = heightEmpty;
    for (size_t i = 0; i < _deckType.GetNumCards(); i++) {
        LPCardGfx pCard = new CardGfx();
        pCard->SetWidth(_widthEmpty);
        pCard->SetHeight(_heightEmpty);
        err = pCard->SetIdx(i, _deckType);
        if (err != NULL) {
            return err;
        }
        _vct_lpCardGfx.push_back(pCard);
    }

    return NULL;
}

void CardStackGfx::CleanUp() {
    TRACE_DEBUG("CleanUp: delete card stack %d\n", _vct_lpCardGfx.size());
    for (VI vi = _vct_lpCardGfx.begin(); vi != _vct_lpCardGfx.end(); ++vi) {
        LPCardGfx pCard = *vi;
        delete pCard;
    }
    _vct_lpCardGfx.clear();
}

void CardStackGfx::Shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_vct_lpCardGfx.begin(), _vct_lpCardGfx.end(), g);
}
void CardStackGfx::Reverse() {
    std::reverse(_vct_lpCardGfx.begin(), _vct_lpCardGfx.end());
}

bool SortRank(const LPCardGfx& l, const LPCardGfx& r) {
    return l->Rank() < r->Rank();
}
void CardStackGfx::Sort() {
    std::sort(_vct_lpCardGfx.begin(), _vct_lpCardGfx.end(), SortRank);
}

void CardStackGfx::PushCard(const LPCardGfx pCard) {
    _vct_lpCardGfx.push_back(pCard);
}
void CardStackGfx::PushStack(LPCardStackGfx pCardstack) {
    for (VI vi = pCardstack->_vct_lpCardGfx.begin();
         vi != pCardstack->_vct_lpCardGfx.end(); ++vi)
        _vct_lpCardGfx.push_back(*vi);
}

LPCardGfx CardStackGfx::PopCard() {
    if (IsEmpty())
        return NULL;

    LPCardGfx pCard = _vct_lpCardGfx.back();
    _vct_lpCardGfx.pop_back();

    return pCard;
}

LPCardStackGfx CardStackGfx::PopStack(int items) {
    if (items > Size())
        items = Size();

    LPCardStackGfx pCardStack = new CardStackGfx();

    while (items > 0) {
        LPCardGfx pCard = PopCard();
        pCardStack->PushCard(pCard);
        items--;
    }
    if (pCardStack->Size() > 1) {
        pCardStack->Reverse();
    }
    return pCardStack;
}

LPCardGfx CardStackGfx::RemoveCard(int index) {
    if (_vct_lpCardGfx.empty())
        return NULL;

    VI vi = _vct_lpCardGfx.begin() + index;
    LPCardGfx card = *vi;
    _vct_lpCardGfx.erase(vi);

    return card;
}

void CardStackGfx::InsertCard(int index, LPCardGfx card) {
    VI vi = _vct_lpCardGfx.begin() + index;
    _vct_lpCardGfx.insert(vi, card);
}

int CardStackGfx::GetCardWherePointIsInside(int x, int y) {
    int i = _vct_lpCardGfx.size() - 1;

    for (VI vi = _vct_lpCardGfx.end() - 1; vi >= _vct_lpCardGfx.begin(); --vi) {
        LPCardGfx pCard = *vi;
        if (pCard->PtInCard(x, y))
            return i;
        i--;
    }

    return -1;
}

LPCardGfx CardStackGfx::GetCard(int index) {
    if (_vct_lpCardGfx.empty())
        return NULL;

    if (index >= _vct_lpCardGfx.size())
        return NULL;

    VI vi = _vct_lpCardGfx.begin() + index;
    LPCardGfx pCard = *vi;

    return pCard;
}

void CardStackGfx::SetCardsFaceUp(bool bval) {
    for (VI vi = _vct_lpCardGfx.begin(); vi != _vct_lpCardGfx.end(); ++vi) {
        LPCardGfx pCard = *vi;
        pCard->SetFaceUp(bval);
    }
}
