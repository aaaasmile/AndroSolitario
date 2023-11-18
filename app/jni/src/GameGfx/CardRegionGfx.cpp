#include "CardRegionGfx.h"

#include "WinTypeGlobal.h"

void CardRegionGfx::InitCardCoords() {
    if (_internalStack.IsEmpty())
        return;

    int x = _xCoord;
    int y = _yCoord;

    int pos = 0;
    int it = 0;

    for (int i = 0; i < _internalStack.Size(); i++) {
        LPCardGfx pCard = _internalStack.Item(i);
        pCard->SetCardLoc(x, y);
        if (_attributes & CRD_3D) {
            if (!((it++) % 4))
                pos++;
            x = (pos * 2) + _xCoord;
            y = pos + _yCoord;
        } else {
            x += _xOffset;
            y += _yOffset;
        }
    }
}
//__________________________________________________________
/*															|
|  Imperfect, doesn't work well with diagonal, reversed,  	|
|  and 3d stacks... Needs tweaking							|
|_________________________________________________________*/

bool CardRegionGfx::PtInStack(int x, int y) {
    if (!(_attributes & CRD_VISIBLE))
        return false;

    if (_attributes & CRD_3D && !IsEmpty()) {
        LPCardGfx pLastCard = _internalStack.Last();
        return pLastCard->PtInCard(x, y);
    }

    int StackWidth = GetStackWidth();
    int StackHeight = GetStackHeight();

    return (x >= _xCoord && x <= _xCoord + StackWidth && y >= _yCoord &&
            y <= _yCoord + StackHeight);
}

bool CardRegionGfx::PtOnTop(int x, int y) {
    if (IsEmpty())
        return false;

    LPCardGfx pLastCard = _internalStack.Last();
    return pLastCard->PtInCard(x, y);
}

int CardRegionGfx::GetOverlapRatio(int x, int y, int w, int h) {
    if (!(_attributes & CRD_DODROP))
        return 0;

    int StackWidth = GetStackWidth();
    int StackHeight = GetStackHeight();

    int wi, hi;

    /*
     _______________________________
    |								|
    |			 _______			|
    |			|		|			|
    |			|		|			|
    |___________|_______|___________|
                |		|
                |_______|
    */
    if (PtInStack(x, y)) {
        if (_xCoord + StackWidth > x + w)
            wi = w;
        else
            wi = (_xCoord + StackWidth) - x;

        hi = (_yCoord + StackHeight) - y;
    }
    /*
         _______________________________
        |								|
     ___|___				 			|
    |	|	|							|
    |	|	|							|
    |	|___|___________________________|
    |		|
    |_______|
    */
    else if (PtInStack(x + w, y)) {
        wi = (x + w) - _xCoord;
        hi = (_yCoord + StackHeight) - y;
    }
    /*
                 _______
                |		|
     ___________|_______|___________
    |			|		|			|
    |			| 		|			|
    |			|_______|			|
    |			 					|
    |_______________________________|
    */
    else if (PtInStack(x, y + h)) {
        if (_xCoord + StackWidth > x + w)
            wi = w;
        else
            wi = (_xCoord + StackWidth) - x;

        hi = (y + h) - _yCoord;
    }
    /*
     _______
    |		|
    |	 ___|___________________________
    |	|	|							|
    |	|	|		 					|
    |___|___|       					|
        |			 					|
        |_______________________________|
    */
    else if (PtInStack(x + w, y + h)) {
        wi = (x + w) - _xCoord;
        hi = (y + h) - _yCoord;
    }
    /*
     _______________________________
    |								|
    |								|
    |								|
    |								|
    |_______________________________|
    */
    else
        return 0;

    return wi * hi;
}

bool CardRegionGfx::CanDrop(LPCardStackGfx stack) {
    if (_internalStack.IsEmpty() &&
        (!(_acceptMode & CRD_DOKING) && !(_acceptMode & CRD_DOACE)))
        return false;
    if (stack->Size() == 0)
        return false;

    // LastCard is the destination stack
    // StartCard is the dragging card that is dropping
    LPCardGfx pStartCard = stack->First();

    if (_internalStack.IsEmpty() && (_acceptMode & CRD_DOKING) &&
        (pStartCard->Rank() != _deckType.GetMaxRank()))
        return false;
    else if (_internalStack.IsEmpty() && (_acceptMode & CRD_DOACE) &&
             (pStartCard->Rank() != 1))
        return false;
    else if (_internalStack.IsEmpty())
        return true;

    LPCardGfx pLastCard = _internalStack.Last();
    TRACE_DEBUG("CanDrop:  %s -> %s\n", pStartCard->String(),
                pLastCard->String());

    if (pLastCard->IsFaceDown())
        return false;

    if (!(_attributes & CRD_DODROP))
        return false;

    if (_acceptMode & CRD_DOALL)
        return true;

    if ((_acceptMode & CRD_DOSINGLE) && (stack->Size() > 1)) {
        return false;
    }

    if ((_acceptMode & CRD_DOOPCOLOR) &&
        ((pStartCard->IsBlack() && pLastCard->IsBlack()) ||
         (pStartCard->IsRed() && pLastCard->IsRed()))) {
        return false;
    }

    if (((_acceptMode & CRD_DOHIGHER) && !(_acceptMode & CRD_DOLOWER)) &&
        (pStartCard->Rank() <= pLastCard->Rank()))
        return false;
    if ((!(_acceptMode & CRD_DOHIGHER) && (_acceptMode & CRD_DOLOWER)) &&
        (pStartCard->Rank() >= pLastCard->Rank()))
        return false;
    if ((!(_acceptMode & CRD_DOHIGHER) && !(_acceptMode & CRD_DOLOWER)) &&
        (pStartCard->Rank() == pLastCard->Rank()))
        return false;
    if (((_acceptMode & CRD_DOHIGHERBY1) && !(_acceptMode & CRD_DOLOWERBY1)) &&
        ((pStartCard->Rank() - pLastCard->Rank()) > 1))
        return false;
    if ((!(_acceptMode & CRD_DOHIGHERBY1) && (_acceptMode & CRD_DOLOWERBY1)) &&
        ((pStartCard->Rank() - pLastCard->Rank()) < -1))
        return false;

    if ((_acceptMode & CRD_DOSUIT) &&
        (pStartCard->Suit() != pLastCard->Suit())) {
        return false;
    }

    return true;
}

int CardRegionGfx::GetStackWidth() {
    if (_internalStack.IsEmpty())
        return _widthEmpty;
    LPCardGfx pStartCard = _internalStack.First();
    LPCardGfx pLastCard = _internalStack.Last();

    return pLastCard->X() + pLastCard->Width() - pStartCard->X();
}

int CardRegionGfx::GetStackHeight() {
    if (_internalStack.IsEmpty())
        return _heightEmpty;

    LPCardGfx pStartCard = _internalStack.First();
    LPCardGfx pLastCard = _internalStack.Last();

    return pLastCard->Y() + pLastCard->Height() - pStartCard->Y();
}
