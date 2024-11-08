#ifndef SOLIATRIO_GFX__H
#define SOLIATRIO_GFX__H

#include <vector>

#include "CardRegionGfx.h"
#include "ErrorInfo.h"
#include "GameSettings.h"
#include "Languages.h"
#include "Traits.h"
#include "WinTypeGlobal.h"

using namespace traits;
class ButtonGfx;
class CurrentTime;
class HighScore;
class MusicManager;

typedef std::vector<CardRegionGfx>::iterator regionVI;

struct DragPileInfo {
    int x;
    int y;
    int width;
    int height;
    LPCardRegionGfx pSrcRegion;
};

const unsigned int CRD_NSYMBOL = 0;
const unsigned int CRD_XSYMBOL = 1;
const unsigned int CRD_OSYMBOL = 2;
const unsigned int CRD_HSYMBOL = 3;

class SolitarioGfx {
    enum eRegionIx {
        DeckPile_Ix = 0,
        Found_Ix1 = 1,
        Found_Ix2 = 2,
        Found_Ix3 = 3,
        Found_Ix4 = 4,
        Found_Ix5 = 5,
        Found_Ix6 = 6,
        Found_Ix7 = 7,
        DeckFaceUp = 8,
        Ace_Ix1 = 9,
        Ace_Ix2 = 10,
        Ace_Ix3 = 11,
        Ace_Ix4 = 12
    };

   public:
    SolitarioGfx();
    ~SolitarioGfx();

    LPErrInApp Initialize(SDL_Surface *s, SDL_Renderer *r, SDL_Window *w,
                          DeckType &dt, LPLanguages planguages,
                          TTF_Font *pfontSmallText, TTF_Font *pfontBigText,
                          SDL_Surface *pSceneBackground,
                          MusicManager *pMusicManager, bool isBlack,
                          HighScore *pHighScore);
    LPErrInApp StartGameLoop();

    int RegionSize(int regionNo) { return _cardRegionList[regionNo].Size(); }
    void CleanUpRegion();

    void CreateRegion(RegionType rtid, unsigned int attribs, unsigned int amode,
                      int dmode, int symbol, int x, int y, int xoffset,
                      int yoffset);
    bool DeleteRegion(LPCardRegionGfx pRegion);

    void InitAllCoords();

    LPCardRegionGfx SelectRegionOnPoint(int x, int y);

    LPErrInApp InitDrag(int x, int y, bool &isInitDrag,
                        LPCardRegionGfx pSrcRegion);
    LPErrInApp InitDrag(LPCardStackGfx CargoStack, int x, int y,
                        bool &isInitDrag, LPCardRegionGfx pSrcRegion);

    void DoDrag(int x, int y);

    LPCardRegionGfx DoDrop();
    LPCardRegionGfx DoDrop(LPCardRegionGfx pDestRegion);

    LPCardRegionGfx FindDropRegion(int Id, LPCardGfx pCard);
    LPCardRegionGfx FindDropRegion(int Id, LPCardStackGfx pStack);

    void DrawStaticScene();
    LPErrInApp DrawInitialScene();

    LPCardRegionGfx GetBestStack(int x, int y, int width, int height,
                                 LPCardStackGfx stack);

    LPErrInApp DrawCardStack(SDL_Surface *s, LPCardRegionGfx pcardRegion);
    LPErrInApp DrawCardStack(LPCardRegionGfx pcardRegion);

    LPErrInApp DrawCard(int x, int y, int nCdIndex);
    LPErrInApp DrawCard(int x, int y, int nCdIndex, SDL_Surface *s);
    LPErrInApp DrawCard(LPCardGfx pCard, SDL_Surface *s);

    LPErrInApp DrawCardPac(int x, int y, int nCdIndex, SDL_Surface *s);
    LPErrInApp DrawCardPac(LPCardGfx pCard, SDL_Surface *s);

    LPErrInApp DrawCardBack(int x, int y);
    LPErrInApp DrawCardBack(int x, int y, SDL_Surface *s);
    LPErrInApp DrawCardBackPac(int x, int y, SDL_Surface *s);

    LPErrInApp DrawSymbol(int x, int y, int nSymbol);
    LPErrInApp DrawSymbol(int x, int y, int nSymbol, SDL_Surface *s);
    LPErrInApp DrawSymbolPac(int x, int y, int nSymbol, SDL_Surface *s);

    LPErrInApp VictoryAnimation();

    LPErrInApp LoadCardPac();
    LPErrInApp LoadSymbolsForPac();

    void SetSymbol(int regionNo, int symbol) {
        _cardRegionList[regionNo].SetSymbol(symbol);
    }
    LPErrInApp NewDeck(int regionNo) {
        return _cardRegionList[regionNo].NewDeck();
    }
    bool IsRegionEmpty(int regionNo) {
        return _cardRegionList[regionNo].IsEmpty();
    }
    void Shuffle(int regionNo) { _cardRegionList[regionNo].Shuffle(); }
    void PushCardInRegion(int regionNo, LPCardGfx pCard) {
        _cardRegionList[regionNo].PushCard(pCard);
    }
    void PushStackInRegion(int regionNo, LPCardStackGfx pStack) {
        _cardRegionList[regionNo].PushStack(pStack);
    }
    LPCardGfx PopCardFromRegion(int regionNo) {
        return _cardRegionList[regionNo].PopCard();
    }
    LPCardStackGfx PopStackFromRegion(int regionNo, int items) {
        return _cardRegionList[regionNo].PopStack(items);
    }
    void SetCardFaceUp(int regionNo, bool bVal, int idx) {
        _cardRegionList[regionNo].SetCardFaceUp(bVal, idx);
    }
    void Reverse(int regionNo) { _cardRegionList[regionNo].Reverse(); }
    void InitCardCoords(int regionNo) {
        _cardRegionList[regionNo].InitCardCoords();
    }
    LPCardRegionGfx GetRegion(int regionNo) {
        return &_cardRegionList[regionNo];
    }
    void BtQuitClick();
    void BtNewGameClick();
    void BtToggleSoundClick();

   private:
    void updateTextureAsFlipScreen();
    void zoomDropCard(int &sx, int &sy, LPCardGfx pCard, int width, int height);
    void setDeckType(DeckType &dt) { _deckType.CopyFrom(dt); }
    void clearSurface();
    LPErrInApp newGame();
    LPErrInApp handleGameLoopKeyDownEvent(SDL_Event &event);
    LPErrInApp handleGameLoopMouseDownEvent(SDL_Event &event);
    LPErrInApp handleGameLoopFingerDownEvent(SDL_Event &event);
    LPErrInApp handleGameLoopFingerUpEvent(SDL_Event &event);
    void handleGameLoopMouseMoveEvent(SDL_Event &event);
    LPErrInApp handleGameLoopMouseUpEvent(SDL_Event &event);
    LPErrInApp handleLeftMouseDown(SDL_Event &event);
    LPErrInApp handleRightMouseDown(SDL_Event &event);
    ClickCb prepClickQuitCb();
    ClickCb prepClickNewGameCb();
    ClickCb prepClickToggleSoundCb();
    LPErrInApp drawScore(SDL_Surface *pScreen);
    void updateScoreOnAce(int sizeAce, int oldSizeAce);
    void updateScoreOnTurnOverFaceDown();
    void updateScoreMoveDeckToTableau();
    void updateBadScoreScoreOnTime();
    void updateBadScoreRedial();
    void updateBadScoreAceToTableu();
    void clearScore();
    void bonusScore();
    int showYesNoMsgBox(LPCSTR strText);
    void showOkMsgBox(LPCSTR strText);

   private:
    CardStackGfx _dragStack;
    DragPileInfo _dragPileInfo;
    LPCardRegionGfx _p_selectedCardRegion;

    SDL_Surface *_p_Screen;
    SDL_Surface *_p_AlphaDisplay;
    SDL_Surface *_p_ScreenBackbufferDrag;
    SDL_Texture *_p_ScreenTexture;
    SDL_Surface *_p_Dragface;
    SDL_Surface *_p_SceneBackground;
    SDL_Renderer *_p_sdlRenderer;
    SDL_Window *_p_Window;
    LPLanguages _p_Languages;
    TTF_Font *_p_FontBigText;
    TTF_Font *_p_FontSmallText;
    HighScore *_p_HighScore;
    MusicManager *_p_MusicManager;

    int _oldx;
    int _oldy;

    SDL_Surface *_p_Deck;
    SDL_Surface *_p_Symbols;
    DeckType _deckType;

    bool _startdrag;

    SDL_Rect _rctSrcCard;
    std::vector<CardRegionGfx> _cardRegionList;
    bool _terminated;
    bool _newgamerequest;
    ButtonGfx *_p_BtQuit;
    ButtonGfx *_p_BtNewGame;
    ButtonGfx *_p_BtToggleSound;
    bool _sceneBackgroundIsBlack;
    int _scoreGame;
    bool _scoreChanged;
    CurrentTime *_p_currentTime;
    Uint32 _lastUpTimestamp;
};

#endif
