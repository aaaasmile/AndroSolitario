#ifndef _MAINOPTION_GFX_H
#define _MAINOPTION_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>

#include "CardGfx.h"
#include "ErrorInfo.h"
#include "GameSettings.h"
#include "Traits.h"
#include "WinTypeGlobal.h"

class ButtonGfx;
class Languages;
class CheckBoxGfx;
class ComboGfx;
class MusicManager;

using namespace traits;

class OptionsGfx {
    enum {
        MYIDOK = 0,
        MYIDCANCEL = 1,
        MYIDMUSICCHK = 2,
        MYIDCOMBOLANG = 3,
        MYIDCOMBODECK = 4,
        MYIDCOMBOBACK = 5
    };

public:
    OptionsGfx();
    ~OptionsGfx();

    LPErrInApp Initialize(SDL_Surface* pScreen,
                          SDL_Renderer* pRenderer, MusicManager* pMusicMgr,
                          MenuDelegator& menuDlg);
    LPErrInApp Show(SDL_Surface* pScene_background, STRING& strCaption);
    void SetCaption(STRING& strCaption) { _headerText = strCaption; }
    LPErrInApp ButEndOPtClicked(int iButID);
    void CheckboxMusicClicked(bool state);

private:
    ClickCb prepClickCb();
    ClickCb prepClickComboCb();
    CheckboxClickCb prepCheckBoxClickMusic();
    ClickCb prepSelectionDeckCb();

private:
    SDL_Renderer* _p_sdlRenderer;
    SDL_Rect _rctOptBox;
    SDL_Surface* _p_surfBar;
    SDL_Surface* _p_screen;
    TTF_Font* _p_fontText;
    TTF_Font* _p_fontCtrl;
    SDL_Color _color;
    ButtonGfx* _p_buttonOK;
    bool _terminated;
    int _result;
    STRING _headerText;
    CheckBoxGfx* _p_checkMusic;
    ComboGfx* _p_comboLang;
    ComboGfx* _p_comboDeck;
    ComboGfx* _p_comboBackground;
    SDL_Surface* _p_deckAll[eDeckType::NUM_OF_DECK];
    CardGfx _cardOnEachDeck[3][eDeckType::NUM_OF_DECK];
    MenuDelegator _menuDlgt;
    GameSettings* _p_GameSettings;
    MusicManager* _p_MusicManager;
};

#endif
