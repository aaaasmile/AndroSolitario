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
class TextInputGfx;
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

    LPErrInApp Initialize(SDL_Surface* pScreen, SDL_Renderer* pRenderer,
                          OptionDelegator& optDlg, SDL_Window* pWindow);
    LPErrInApp Show(SDL_Surface* pScene_background, STRING& strCaption);
    LPErrInApp HandleEvent(SDL_Event* pEvent);
    LPErrInApp HandleIterate(bool& done);
    void SetCaption(STRING& strCaption) { _headerText = strCaption; }
    LPErrInApp ButEndOPtClicked(int iButID);
    void CheckboxMusicClicked(bool state);
    void Reset() { _inProgress = false; }
    bool IsInProgress() { return _inProgress; }

   private:
    ClickCb prepClickCb();
    CheckboxClickCb prepCheckBoxClickMusic();
    
   private:
    SDL_Renderer* _p_sdlRenderer;
    SDL_Rect _rctOptBox;
    SDL_Surface* _p_surfBar;
    SDL_Surface* _p_screen;
    TTF_Font* _p_fontText;
    TTF_Font* _p_fontCtrl;
    SDL_Color _color;
    ButtonGfx* _p_buttonOK;
    STRING _headerText;
    CheckBoxGfx* _p_checkMusic;
    ComboGfx* _p_comboLang;
    ComboGfx* _p_comboDeck;
    ComboGfx* _p_comboBackground;
    TextInputGfx* _p_textInput;
    SDL_Surface* _p_deckAll[eDeckType::NUM_OF_DECK];
    CardGfx _cardOnEachDeck[3][eDeckType::NUM_OF_DECK];
    OptionDelegator _optDlgt;
    GameSettings* _p_GameSettings;
    MusicManager* _p_MusicManager;
    bool _inProgress;
    bool _mouseDownRec;
    SDL_Texture* _p_ScreenTexture;
    SDL_Surface* _p_ShadowSrf;
    int _hbar;
    int _labelOffsetY;
    int _captionOffsetX;
};

#endif
