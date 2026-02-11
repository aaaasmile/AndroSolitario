#ifndef CARDGFX__H
#define CARDGFX__H

#include <SDL3/SDL.h>

#include <vector>

#include "Config.h"
#include "DeckType.h"
#include "ErrorInfo.h"

class CardGfx {
   public:
    enum eCardState {
        CSW_ST_INVISIBLE = 0,
        CSW_ST_SYMBOL,
        CSW_ST_VISIBLE,
        CSW_ST_BACK
    };
    CardGfx();
    eSUIT Suit() const { return _eSuit; }
    const char* SuitStr();
    int Rank() const { return _rank; }

    bool IsBlack() const { return _eSuit == BASTONI || _eSuit == SPADE; }
    bool IsRed() const { return !IsBlack(); }
    int X() { return _x; }
    int Y() { return _y; }
    int Width() {
        if (_scaleFactor != 0.0)
            return (int)(_width * _scaleFactor);
        return _width;
    }
    void SetWidth(int val) { _width = val; }
    void SetHeight(int val) { _height = val; }
    int Height() {
        if (_scaleFactor != 0.0)
            return (int)(_height * _scaleFactor);
        return _height;
    }
    void SetDeckSurface(SDL_Surface* pVal) { _pPacDeck = pVal; }

    bool IsFaceUp() const { return _faceUp; }
    bool IsFaceDown() const { return !_faceUp; }
    LPCSTR Name() { return _name.c_str(); }
    LPCSTR String();

    void SetFaceUp(bool bval) { _faceUp = bval; }
    LPErrInApp DrawCardPac(SDL_Surface* s);

    int Index() { return _idx; }
    LPErrInApp SetIdx(int nIdx, DeckType& deckType);
    void SetCardLoc(int lx, int ly) {
        _x = lx;
        _y = ly;
    }

    bool PtInCard(int lx, int ly) {
        SDL_assert(_width != 0);
        SDL_assert(_height != 0);
        int w = Width();
        int h = Height();
        if (lx >= _x && lx <= w + _x && ly >= _y && ly <= h + _y)
            return true;
        else
            return false;
    }
    void SetScaleFactor(float factor) { _scaleFactor = factor; }
    float ScaleFactor() { return _scaleFactor; }

    void DrawCard(SDL_Surface* s) { DrawCardPac(s); }
    void DrawCardBack(SDL_Surface* s);
    void DrawSymbol(SDL_Surface* s);
    void SetSymbSurf(SDL_Surface* pSurf, int w, int h);
    void Copy(CardGfx* pSrc);
    bool MouseInCard(int x, int y) { return PtInCard(x, y); }

    int _x;
    int _y;
    int _vx;
    int _vy;
    eCardState State;
    int m_iZOrder;

   private:
    bool _faceUp;
    eSUIT _eSuit;
    int _rank;

    int _idx;
    int _width;
    int _height;

    SDL_Surface* _pPacDeck;
    SDL_Surface* _pSymbSurf;
    int _symbWidth;
    int _symbHeigth;
    DeckType _deckType;
    STRING _name;
    STRING _nameFull;
    float _scaleFactor;
};

typedef CardGfx* LPCardGfx;
typedef std::vector<LPCardGfx>::iterator VI;

#endif
