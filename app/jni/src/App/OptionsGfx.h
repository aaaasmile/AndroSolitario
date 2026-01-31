#ifndef _MAINOPTION_GFX_H
#define _MAINOPTION_GFX_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "CardGfx.h"
#include "Config.h"
#include "ErrorInfo.h"
#include "GameSettings.h"
#include "Traits.h"

class ButtonGfx;
class Languages;
class CheckBoxGfx;
class ComboGfx;
class TextInputGfx;
class MusicManager;
class KeyboardGfx;

using namespace traits;

class OptionsGfx {
   public:
    enum {
        MYIDOK = 0,
        MYIDCANCEL = 1,
        MYIDMUSICCHK = 2,
        MYIDCOMBOLANG = 3,
        MYIDCOMBODECK = 4,
        MYIDCOMBOBACK = 5,
        MYIDKYB = 6
    };

    OptionsGfx();
    ~OptionsGfx();

    LPErrInApp Initialize(SDL_Surface* pScreen, UpdateScreenCb& fnUpdateScreen,
                          OptionDelegator& optDlg, SDL_Window* pWindow);
    LPErrInApp Show(SDL_Surface* pScene_background, STRING& strCaption);
    LPErrInApp HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos);
    LPErrInApp HandleIterate(bool& done);
    void SetCaption(STRING& strCaption) { _headerText = strCaption; }
    LPErrInApp OptionsEnd();
    void CheckboxMusicClicked(bool state);
    void Reset() { _inProgress = false; }
    bool IsOngoing() { return _inProgress; }
    LPErrInApp UpdateScreen(SDL_Surface* pScreen,
                            SDL_Surface* pScene_background);
    void ToggleScreenKeyboard();
    void TextFromKeyboard(const char* text);

   private:
    ClickCb prepClickCb();
    CheckboxClickCb prepCheckBoxClickMusic();
    ClickKeyboardCb prepareClickKeyboardCb();
    void setBackgoundTypeInGameSettings();
    void setLanguageInGameSettings();
    void setControlLocalCaptions();

   private:
    SDL_Rect _rctOptBox;
    SDL_Surface* _p_surfBar;
    SDL_Surface* _p_screen;
    TTF_Font* _p_fontText;
    TTF_Font* _p_fontCtrl;
    ButtonGfx* _p_buttonOK;
    ButtonGfx* _p_btToggleKeyboard;
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
    SDL_Surface* _p_Scene_background;
    bool _inProgress;
    bool _mouseDownRec;
    UpdateScreenCb _fnUpdateScreen;

    SDL_Surface* _p_ShadowSrf;
    int _hbar;
    int _labelOffsetY;
    int _captionOffsetX;
    Languages::eLangId _prevLangId;
    eDeckType _prevDeckType;
    bool _prevMusicEnabled;
    BackgroundTypeEnum _prevBackgroundType;
    std::string _prevName;
    KeyboardGfx* _p_KeyboardGfx;
    SDL_Window* _p_Window;
};

#endif
