#include "OptionsGfx.h"

#include <SDL3/SDL_endian.h>
#include <SDL3_image/SDL_image.h>

#include "CompGfx/ButtonGfx.h"
#include "CompGfx/CheckBoxGfx.h"
#include "CompGfx/ComboGfx.h"
#include "CompGfx/KeyboardGfx.h"
#include "CompGfx/TextInputGfx.h"
#include "Config.h"
#include "GfxUtil.h"
#include "MusicManager.h"

OptionsGfx::OptionsGfx() {
    _p_screen = NULL;
    _p_fontText = NULL;
    _p_surfBar = NULL;
    _p_buttonOK = NULL;
    _p_MusicManager = NULL;
    _p_GameSettings = GameSettings::GetSettings();
    _mouseDownRec = false;
    _fnUpdateScreen.tc = NULL;
    _fnUpdateScreen.self = NULL;
    _p_ShadowSrf = NULL;
    _inProgress = false;
    _p_checkMusic = NULL;
    _p_comboLang = NULL;
    _p_comboDeck = NULL;
    _p_comboBackground = NULL;
    _p_textInput = NULL;
    _p_Scene_background = NULL;
    _p_btToggleKeyboard = NULL;
    _p_KeyboardGfx = NULL;
}

OptionsGfx::~OptionsGfx() {
    delete _p_buttonOK;
    delete _p_btToggleKeyboard;
    delete _p_comboLang;
    delete _p_checkMusic;
    delete _p_comboDeck;
    delete _p_comboBackground;
    delete _p_textInput;
    if (_p_KeyboardGfx != NULL) {
        delete _p_KeyboardGfx;
        _p_KeyboardGfx = NULL;
    }

    for (int i = 0; i < eDeckType::NUM_OF_DECK; i++) {
        if (_p_deckAll[i]) {
            SDL_DestroySurface(_p_deckAll[i]);
        }
    }
    if (_p_surfBar != NULL) {
        SDL_DestroySurface(_p_surfBar);
        _p_surfBar = NULL;
    }
    if (_p_ShadowSrf != NULL) {
        SDL_DestroySurface(_p_ShadowSrf);
        _p_ShadowSrf = NULL;
    }
}

// Prepare the Click() trait
void fncBind_ButtonClicked(void* self, int iVal) {
    OptionsGfx* pOptionsGfx = (OptionsGfx*)self;
    switch (iVal) {
        case OptionsGfx::MYIDOK:
            pOptionsGfx->OptionsEnd();
            break;
        case OptionsGfx::MYIDKYB:
            pOptionsGfx->ToggleScreenKeyboard();
            break;
        default:
            TRACE_DEBUG("Ignore bt key id %d \n", iVal);
            break;
    }
}

// Buttons, ok, show keyboard
ClickCb OptionsGfx::prepClickCb() {
#ifndef _MSC_VER
    static VClickCb const tc = {.Click = (&fncBind_ButtonClicked)};

    return (ClickCb){.tc = &tc, .self = this};
#else
    static VClickCb const tc = {(&fncBind_ButtonClicked)};
    ClickCb cb = {&tc, this};
    return cb;
#endif
}

void fncBind_CheckboxMusicClicked(void* self, bool state) {
    OptionsGfx* pOptionsGfx = (OptionsGfx*)self;
    pOptionsGfx->CheckboxMusicClicked(state);
}

// Checkbox music
CheckboxClickCb OptionsGfx::prepCheckBoxClickMusic() {
#ifndef _MSC_VER
    static VCheckboxClickCb const tc = {.Click =
                                            (&fncBind_CheckboxMusicClicked)};

    return (CheckboxClickCb){.tc = &tc, .self = this};
#else
    static VCheckboxClickCb const tc = {(&fncBind_CheckboxMusicClicked)};
    CheckboxClickCb cb = {&tc, this};
    return cb;
#endif
}

LPErrInApp OptionsGfx::Initialize(SDL_Surface* pScreen,
                                  UpdateScreenCb& fnUpdateScreen,
                                  OptionDelegator& optDlg,
                                  SDL_Window* pWindow) {
    if (pScreen == NULL) {
        return ERR_UTIL::ErrorCreate("pScreen is null");
    }
    if (_p_screen != NULL) {
        if (pScreen->w == _p_screen->w && pScreen->h == _p_screen->h) {
            return NULL;
        }
    }

    _rctOptBox.w = 600;
    _rctOptBox.h = 580;
    _rctOptBox.x = (pScreen->w - _rctOptBox.w) / 2;
    _rctOptBox.y = (pScreen->h - _rctOptBox.h) / 2;
    TRACE_DEBUG("rctOptBox is x: %d, y: %d w: %d, h: %d \n", _rctOptBox.x,
                _rctOptBox.y, _rctOptBox.w, _rctOptBox.h);

    _p_screen = pScreen;
    _optDlgt = optDlg;
    _p_MusicManager = _p_GameSettings->GetMusicManager();
    _p_fontCtrl = _p_GameSettings->GetFontAriblk();
    _p_fontText = _p_GameSettings->GetFontMedium();
    _fnUpdateScreen = fnUpdateScreen;

    _p_surfBar = GFX_UTIL::SDL_CreateRGBSurface(_rctOptBox.w, _rctOptBox.h, 32,
                                                0, 0, 0, 0);
    if (_p_surfBar == NULL) {
        return ERR_UTIL::ErrorCreate("_p_surfBar error: %s\n", SDL_GetError());
    }

    SDL_FillSurfaceRect(
        _p_surfBar, NULL,
        SDL_MapRGB(SDL_GetPixelFormatDetails(_p_surfBar->format), NULL, 10, 10,
                   10));

    SDL_SetSurfaceBlendMode(_p_surfBar, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_surfBar, 70);

    SDL_Rect rctBt1;
    int iSpace2bt = 20;
    ClickCb cbBtClicked = prepClickCb();

    // button OK
    _p_buttonOK = new ButtonGfx();
    rctBt1.w = 120;
    rctBt1.h = 34;
    int offsetBtY = 30;
    rctBt1.y = _rctOptBox.y + _rctOptBox.h - offsetBtY - rctBt1.h;
    rctBt1.x =
        (_rctOptBox.w - rctBt1.w) / 2 + _rctOptBox.x + rctBt1.w + iSpace2bt;
    _p_buttonOK->Initialize(&rctBt1, pScreen, _p_fontText, MYIDOK, cbBtClicked);
    _p_buttonOK->SetVisibleState(ButtonGfx::INVISIBLE);
    // Combo
    int comboW = 220;
    int comboH = 36;
    int comboOffsetY = 80;
    int combo2OffsetY = 20;
    int combo3OffsetY = 40;
    int comboOffsetX = 50;
    // Combo language
    ClickCb nullCb = {NULL, NULL};

    _p_comboLang = new ComboGfx();
    rctBt1.w = comboW;
    rctBt1.h = comboH;
    rctBt1.y = _rctOptBox.y + comboOffsetY;
    rctBt1.x = _rctOptBox.x + comboOffsetX;
    _p_comboLang->Initialize(&rctBt1, pScreen, _p_fontText, MYIDCOMBOLANG,
                             _fnUpdateScreen, nullCb);
    _p_comboLang->SetVisibleState(ComboGfx::INVISIBLE);
    // Music
    // check box music
    CheckboxClickCb cbCheckboxMusic = prepCheckBoxClickMusic();
    _p_checkMusic = new CheckBoxGfx();
    rctBt1.w = comboW;
    rctBt1.h = comboH;
    rctBt1.y = _p_comboLang->PosY() + _p_comboLang->Height() + combo2OffsetY;
    rctBt1.x = _p_comboLang->PosX();
    _p_checkMusic->Initialize(&rctBt1, pScreen, _p_fontText, MYIDMUSICCHK,
                              cbCheckboxMusic);
    _p_checkMusic->SetVisibleState(CheckBoxGfx::INVISIBLE);
    // game image background
    _p_comboBackground = new ComboGfx();
    rctBt1.w = comboW;
    rctBt1.h = comboH;
    rctBt1.y = _p_checkMusic->PosY() + _p_checkMusic->Height() + combo2OffsetY;
    rctBt1.x = _p_checkMusic->PosX();
    _p_comboBackground->Initialize(&rctBt1, pScreen, _p_fontText, MYIDCOMBOBACK,
                                   _fnUpdateScreen, nullCb);
    _p_comboBackground->SetVisibleState(ComboGfx::INVISIBLE);
    // Player name
    _p_textInput = new TextInputGfx();
    rctBt1.w = comboW;
    rctBt1.h = comboH;
    rctBt1.y = _p_comboBackground->PosY() + _p_comboBackground->Height() +
               combo2OffsetY;
    rctBt1.x = _p_comboBackground->PosX();
    _p_textInput->Initialize(&rctBt1, pScreen, _p_fontText, pWindow);

    // button keyboard
    std::string strSymbKeyb = "⌨";
    SDL_Rect rctBtK;
    _p_btToggleKeyboard = new ButtonGfx();
    rctBtK.w = 60;
    rctBtK.h = 34;
    rctBtK.y = rctBt1.y;
    rctBtK.x = rctBt1.x + rctBt1.w + iSpace2bt;
    _p_btToggleKeyboard->InitializeAsSymbol(
        &rctBtK, pScreen, _p_GameSettings->GetFontSymb(), MYIDKYB, cbBtClicked);
    _p_btToggleKeyboard->SetButtonText(strSymbKeyb.c_str());
    _p_btToggleKeyboard->SetVisibleState(ButtonGfx::INVISIBLE);

    // Deck
    // combo deck selection
    _p_comboDeck = new ComboGfx();
    rctBt1.w = comboW;
    rctBt1.h = comboH;
    rctBt1.y = _p_textInput->PosY() + _p_textInput->Height() + combo3OffsetY;
    rctBt1.x = _p_textInput->PosX();

    _p_comboDeck->Initialize(&rctBt1, pScreen, _p_fontText, MYIDCOMBODECK,
                             _fnUpdateScreen, nullCb);
    _p_comboDeck->SetVisibleState(ComboGfx::INVISIBLE);

    // init surfaces with all pac decks
    Uint16 pac_w, pac_h;
    DeckType dt;
    LPErrInApp err;
    for (int i = 0; i < eDeckType::NUM_OF_DECK; i++) {
        err = dt.SetTypeIndex(i);
        if (err != NULL) {
            return err;
        }
        err = GFX_UTIL::LoadCardPac(&_p_deckAll[i], dt, &pac_w, &pac_h);
        if (err != NULL) {
            return err;
        }
        int ww = pac_w / 4;
        int hh = pac_h / dt.GetNumCardInSuit();
        if (ww <= 0) {
            return ERR_UTIL::ErrorCreate("Deck width is zero: %s\n",
                                         dt.GetDeckName().c_str());
        }
        if (hh <= 0) {
            return ERR_UTIL::ErrorCreate("Deck height is zero: %s\n",
                                         dt.GetDeckName().c_str());
        }
        int x_pos = rctBt1.x;
        int y_pos = rctBt1.y + rctBt1.h + 20;
        _cardOnEachDeck[0][i].SetIdx(9, dt);
        _cardOnEachDeck[0][i].SetWidth(ww);
        _cardOnEachDeck[0][i].SetHeight(hh);
        _cardOnEachDeck[0][i].SetCardLoc(x_pos, y_pos);
        _cardOnEachDeck[0][i].SetDeckSurface(_p_deckAll[i]);

        _cardOnEachDeck[1][i].SetIdx(7, dt);
        _cardOnEachDeck[1][i].SetWidth(ww);
        _cardOnEachDeck[1][i].SetHeight(hh);
        _cardOnEachDeck[1][i].SetCardLoc(_cardOnEachDeck[0][i].X() + 10 + ww,
                                         y_pos);
        _cardOnEachDeck[1][i].SetDeckSurface(_p_deckAll[i]);

        _cardOnEachDeck[2][i].SetIdx(5, dt);
        _cardOnEachDeck[2][i].SetWidth(ww);
        _cardOnEachDeck[2][i].SetHeight(hh);
        _cardOnEachDeck[2][i].SetCardLoc(_cardOnEachDeck[1][i].X() + 10 + ww,
                                         y_pos);
        _cardOnEachDeck[2][i].SetDeckSurface(_p_deckAll[i]);

        SDL_Surface* surface = _p_deckAll[i];
        if (dt.GetType() == eDeckType::TAROCK_PIEMONT) {
            TRACE_DEBUG("[TAROCK_PIEMONT] Deck format: %s, w: %d, h: %d\n",
                        SDL_GetPixelFormatName(surface->format), surface->w,
                        surface->h);
            TRACE_DEBUG(
                "[TAROCK_PIEMONT] pScreen buffer format: %s, w: %d, h: %d\n",
                SDL_GetPixelFormatName(pScreen->format), pScreen->w,
                pScreen->h);
            float factor = 0.7;
            for (int j = 0; j < 3; j++) {
                _cardOnEachDeck[j][i].SetScaleFactor(factor);
                if (j > 0) {
                    _cardOnEachDeck[j][i].SetCardLoc(
                        _cardOnEachDeck[j - 1][i].X() + 10 + (int)(ww * factor),
                        y_pos);
                }
            }
        }
    }

    TRACE_DEBUG("Options - Initialized OK\n");
    return NULL;
}

LPErrInApp OptionsGfx::HandleEvent(SDL_Event* pEvent,
                                   const SDL_Point& targetPos) {
    LPErrInApp err;
    if (pEvent->type == SDL_EVENT_QUIT) {
        _inProgress = false;
    }
    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_RETURN) {
            if (!_p_textInput->GetHasFocus()) {
                err = OptionsEnd();
                if (err)
                    return err;
            }
        } else if (pEvent->key.key == SDLK_ESCAPE) {
            err = OptionsEnd();
            if (err)
                return err;
        }
    }
    if (_p_KeyboardGfx != NULL) {
        _p_KeyboardGfx->HandleEvent(pEvent, targetPos);
        if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
            _p_btToggleKeyboard->MouseUp(pEvent, targetPos);
        }
        return NULL;
    }
#if HASTOUCH
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        _p_buttonOK->FingerDown(pEvent);
        _p_btToggleKeyboard->FingerDown(pEvent);
        _p_checkMusic->FingerDown(pEvent);
        _p_comboLang->FingerDown(pEvent);
        _p_comboBackground->FingerDown(pEvent);
        _p_comboDeck->FingerDown(pEvent);
    }
#endif
#if HASMOUSE
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        _mouseDownRec = true;
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (_mouseDownRec) {
            _p_buttonOK->MouseUp(pEvent, targetPos);
            _p_btToggleKeyboard->MouseUp(pEvent, targetPos);
            _p_comboLang->MouseUp(pEvent, targetPos);
            _p_checkMusic->MouseUp(pEvent, targetPos);
            _p_comboBackground->MouseUp(pEvent, targetPos);
            _p_comboDeck->MouseUp(pEvent, targetPos);
            _mouseDownRec = false;
        }
    }
    if (pEvent->type == SDL_EVENT_MOUSE_MOTION) {
        _p_buttonOK->MouseMove(pEvent, targetPos);
        _p_btToggleKeyboard->MouseMove(pEvent, targetPos);
        _p_comboLang->MouseMove(pEvent, targetPos);
        _p_comboBackground->MouseMove(pEvent, targetPos);
        _p_comboDeck->MouseMove(pEvent, targetPos);
    }
#endif
    _p_textInput->HandleEvent(pEvent, targetPos);

    return NULL;
}

LPErrInApp OptionsGfx::HandleIterate(bool& done) {
    LPErrInApp err;
    SDL_Rect clipRect;
    SDL_GetSurfaceClipRect(_p_ShadowSrf, &clipRect);
    SDL_FillSurfaceRect(
        _p_ShadowSrf, &clipRect,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(_p_ShadowSrf->format), NULL, 0, 0,
                    0, 0));
    if (_p_comboLang->GetSelectedIndex() != _p_GameSettings->CurrentLanguage) {
        TRACE_DEBUG("Language selection changed to %d \n",
                    _p_comboLang->GetSelectedIndex());
        setLanguageInGameSettings();
        _p_GameSettings->SetCurrentLang();
        setControlLocalCaptions();
    }
    if (_p_comboBackground->GetSelectedIndex() !=
        _p_GameSettings->BackgroundType) {
        TRACE_DEBUG("Backaground selection changed to %d \n",
                    _p_comboBackground->GetSelectedIndex());
        setBackgoundTypeInGameSettings();

        err = _optDlgt.tc->ChangeSceneBackground(_optDlgt.self,
                                                 &_p_Scene_background);
        if (err != NULL) {
            return err;
        }
    }
    //  center the background
    SDL_Rect rctTarget;
    rctTarget.x = (_p_ShadowSrf->w - _p_Scene_background->w) / 2;
    rctTarget.y = (_p_ShadowSrf->h - _p_Scene_background->h) / 2;
    rctTarget.w = _p_Scene_background->w;
    rctTarget.h = _p_Scene_background->h;
    SDL_BlitSurface(_p_Scene_background, NULL, _p_ShadowSrf, &rctTarget);

    // option box background
    GFX_UTIL::DrawStaticSpriteEx(_p_ShadowSrf, 0, 0, _rctOptBox.w, _rctOptBox.h,
                                 _rctOptBox.x, _rctOptBox.y, _p_surfBar);
    // draw border
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctOptBox.x - 1, _rctOptBox.y - 1,
                       _rctOptBox.x + _rctOptBox.w + 1,
                       _rctOptBox.y + _rctOptBox.h + 1, GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctOptBox.x - 2, _rctOptBox.y - 2,
                       _rctOptBox.x + _rctOptBox.w + 2,
                       _rctOptBox.y + _rctOptBox.h + 2, GFX_UTIL_COLOR::Black);
    GFX_UTIL::DrawRect(_p_ShadowSrf, _rctOptBox.x, _rctOptBox.y,
                       _rctOptBox.x + _rctOptBox.w, _rctOptBox.y + _rctOptBox.h,
                       _color);

    // header bar
    SDL_Rect rectHeader;
    Uint32 colorHeader = SDL_MapRGB(
        SDL_GetPixelFormatDetails(_p_screen->format), NULL, 153, 202, 51);
    rectHeader.x = _rctOptBox.x + 1;
    rectHeader.y = _rctOptBox.y + 1;
    rectHeader.h = _hbar;
    rectHeader.w = _rctOptBox.w - 1;
    SDL_FillSurfaceRect(_p_ShadowSrf, &rectHeader, colorHeader);

    GFX_UTIL::DrawStaticLine(
        _p_ShadowSrf, rectHeader.x, rectHeader.y + rectHeader.h,
        rectHeader.x + rectHeader.w, rectHeader.y + rectHeader.h,
        GFX_UTIL_COLOR::White);
    // text header
    GFX_UTIL::DrawString(_p_ShadowSrf, _headerText.c_str(),
                         rectHeader.x + _captionOffsetX, rectHeader.y,
                         GFX_UTIL_COLOR::White, _p_fontCtrl);

    // Button OK
    _p_buttonOK->DrawButton(_p_ShadowSrf);

    // Combo Language: Label and crontrol
    LPLanguages pLanguages = _p_GameSettings->GetLanguageMan();
    STRING strSelectLanguage =
        pLanguages->GetStringId(Languages::ID_CHOOSELANGUA);
    GFX_UTIL::DrawString(_p_ShadowSrf, strSelectLanguage.c_str(),
                         _p_comboLang->PosX(),
                         _p_comboLang->PosY() - _labelOffsetY,
                         GFX_UTIL_COLOR::CustomLbl, _p_fontText);

    _p_comboLang->DrawButton(_p_ShadowSrf);

    // Checkbox music
    _p_checkMusic->DrawButton(_p_ShadowSrf);

    // Combo Background: Label and control
    STRING strSelectBackGround =
        pLanguages->GetStringId(Languages::ID_CHOOSEBACKGROUND);
    GFX_UTIL::DrawString(_p_ShadowSrf, strSelectBackGround.c_str(),
                         _p_comboBackground->PosX(),
                         _p_comboBackground->PosY() - _labelOffsetY,
                         GFX_UTIL_COLOR::CustomLbl, _p_fontText);
    _p_comboBackground->DrawButton(_p_ShadowSrf);

    // player name
    _p_textInput->DrawCtrl(_p_ShadowSrf);
    _p_btToggleKeyboard->DrawButton(_p_ShadowSrf);

    // Combo Deck: Label and control
    STRING strDeckSelectTitle =
        pLanguages->GetStringId(Languages::ID_CHOOSEDECK);
    GFX_UTIL::DrawString(_p_ShadowSrf, strDeckSelectTitle.c_str(),
                         _p_comboDeck->PosX(),
                         _p_comboDeck->PosY() - _labelOffsetY,
                         GFX_UTIL_COLOR::CustomLbl, _p_fontText);

    _p_comboDeck->DrawButton(_p_ShadowSrf);

    // Deck example Cards
    int iCurrIndex = _p_comboDeck->GetSelectedIndex();
    if (iCurrIndex < 0 || iCurrIndex >= eDeckType::NUM_OF_DECK) {
        return ERR_UTIL::ErrorCreate("i out of range: %d", iCurrIndex);
    }
    _cardOnEachDeck[0][iCurrIndex].DrawCardPac(_p_ShadowSrf);
    _cardOnEachDeck[1][iCurrIndex].DrawCardPac(_p_ShadowSrf);
    _cardOnEachDeck[2][iCurrIndex].DrawCardPac(_p_ShadowSrf);

    if (_p_KeyboardGfx != NULL) {
        _p_KeyboardGfx->DrawCtrl(_p_ShadowSrf);
    }

    // render the dialogbox
    SDL_BlitSurface(_p_ShadowSrf, NULL, _p_screen, NULL);
    (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_screen);

    _p_textInput->Update();

    if (!_inProgress) {
        // time to leave
        if (_p_ShadowSrf != NULL) {
            SDL_DestroySurface(_p_ShadowSrf);
            _p_ShadowSrf = NULL;
        }
        done = true;
    }
    return NULL;
}

void fncBind_KeyboardClick(void* self, const char* text) {
    OptionsGfx* pOptionsGfx = (OptionsGfx*)self;
    pOptionsGfx->TextFromKeyboard(text);
}

ClickKeyboardCb OptionsGfx::prepareClickKeyboardCb() {
#ifndef _MSC_VER
    static VClickKeyboardCb const tc = {.ClickKey = (&fncBind_KeyboardClick)};

    return (ClickKeyboardCb){.tc = &tc, .self = this};
#else
    static VClickKeyboardCb const tc = {(&fncBind_KeyboardClick)};
    ClickKeyboardCb cb = {&tc, this};
    return cb;
#endif
}

void OptionsGfx::TextFromKeyboard(const char* text) {
    std::string currText = _p_textInput->GetText();
    if (text[0] == '\b') {
        if (!currText.empty()) {
            currText.pop_back();
            _p_textInput->SetText(currText);
            _p_textInput->SetHasFocus(true);
        }
        return;
    } else if (text[0] == '\n') {
        if (_p_KeyboardGfx != NULL) {
            ToggleScreenKeyboard();
            _p_textInput->SetHasFocus(false);
        }
        return;
    }
    std::string newText = currText + std::string(text);
    _p_textInput->SetText(newText);
    _p_textInput->SetHasFocus(true);
}

void OptionsGfx::ToggleScreenKeyboard() {
    if (_p_KeyboardGfx != NULL) {
        TRACE_DEBUG("[ToggleScreenKeyboard] hide keyboard \n");
        delete _p_KeyboardGfx;
        _p_KeyboardGfx = NULL;
        return;
    }
    TRACE_DEBUG("[ToggleScreenKeyboard] show keyboard \n");
    _p_KeyboardGfx = new KeyboardGfx();
    SDL_Rect rctKeyboard;
    rctKeyboard.x = 2;
    rctKeyboard.y = (_p_screen->h / 2) + 100;
    rctKeyboard.w = _p_screen->w - 2;
    rctKeyboard.h = (_p_screen->h / 2) - 100;

    ClickKeyboardCb cbKeyboard = prepareClickKeyboardCb();
    _p_KeyboardGfx->Show(&rctKeyboard, _p_screen,
                         _p_GameSettings->GetFontMedium(), cbKeyboard);
}

LPErrInApp OptionsGfx::Show(SDL_Surface* pScene_background,
                            STRING& strCaption) {
    TRACE_DEBUG("Options - Show\n");
    if (_inProgress) {
        return ERR_UTIL::ErrorCreate(
            "Fade is already in progess, use iterate\n");
    }
    _inProgress = true;
    _p_Scene_background = pScene_background;
    _headerText = strCaption;
    _prevLangId = _p_GameSettings->CurrentLanguage;
    _prevDeckType = _p_GameSettings->DeckTypeVal.GetType();
    _prevMusicEnabled = _p_GameSettings->MusicEnabled;
    _prevBackgroundType = _p_GameSettings->BackgroundType;
    _prevName = _p_GameSettings->PlayerName;

    // combo language
    setControlLocalCaptions();

    _p_comboLang->SetVisibleState(ComboGfx::VISIBLE);
    _p_comboLang->SelectIndex(_p_GameSettings->CurrentLanguage);

    // Button ok
    _p_buttonOK->SetVisibleState(ButtonGfx::VISIBLE);
    _p_btToggleKeyboard->SetVisibleState(ButtonGfx::VISIBLE);

    // checkbox music
    _p_checkMusic->SetVisibleState(CheckBoxGfx::VISIBLE);
    _p_checkMusic->SetCheckState(_p_GameSettings->MusicEnabled);

    // combobox background selection
    _p_comboBackground->SetVisibleState(ComboGfx::VISIBLE);
    _p_comboBackground->SelectIndex(_p_GameSettings->BackgroundType);

    // player name
    _p_textInput->SetText(_p_GameSettings->PlayerName);
    _p_textInput->SetVisibleState(TextInputGfx::VISIBLE);

    // combobox deck selection
    _p_comboDeck->ClearLines();
    DeckType dt;
    STRING deckName;
    for (int i = 0; i < eDeckType::NUM_OF_DECK; i++) {
        dt.SetTypeIndex(i);
        deckName = dt.GetDeckName();
        _p_comboDeck->AddLineText(deckName.c_str());
    }
    _p_comboDeck->SetVisibleState(ComboGfx::VISIBLE);
    _p_comboDeck->SelectIndex(_p_GameSettings->DeckTypeVal.GetTypeIndex());

    _hbar = 30;
    _captionOffsetX = 10;
    _labelOffsetY = 25;
    if (_p_ShadowSrf != NULL) {
        SDL_DestroySurface(_p_ShadowSrf);
    }
    _p_ShadowSrf = GFX_UTIL::SDL_CreateRGBSurface(_p_screen->w, _p_screen->h,
                                                  32, 0, 0, 0, 0);

    TRACE_DEBUG(
        "[TAROCK_PIEMONT] _p_ShadowSrf buffer format: %s, w: %d, h: %d\n",
        SDL_GetPixelFormatName(_p_ShadowSrf->format), _p_ShadowSrf->w,
        _p_ShadowSrf->h);
    return NULL;
}

void OptionsGfx::setControlLocalCaptions() {
    LPLanguages pLanguages = _p_GameSettings->GetLanguageMan();
    STRING strTextBt;
    // Commbo languages
    _p_comboLang->ClearLines();
    strTextBt = pLanguages->GetStringId(Languages::ID_ITALIANO);
    _p_comboLang->AddLineText(strTextBt.c_str());
    strTextBt = pLanguages->GetStringId(Languages::ID_DIALETMN);
    _p_comboLang->AddLineText(strTextBt.c_str());
    strTextBt = pLanguages->GetStringId(Languages::ID_ENGLISH);
    _p_comboLang->AddLineText(strTextBt.c_str());

    // Button OK
    strTextBt = pLanguages->GetStringId(Languages::ID_OK);
    _p_buttonOK->SetButtonText(strTextBt.c_str());

    // checkbox music
    strTextBt = pLanguages->GetStringId(Languages::ID_SOUNDOPT);
    _p_checkMusic->SetWindowText(strTextBt.c_str());

    // combobox background selection
    _p_comboBackground->ClearLines();
    strTextBt = pLanguages->GetStringId(Languages::ID_COMMESSAGGIO);
    _p_comboBackground->AddLineText(strTextBt.c_str());
    strTextBt = pLanguages->GetStringId(Languages::ID_MANTOVA);
    _p_comboBackground->AddLineText(strTextBt.c_str());
    strTextBt = pLanguages->GetStringId(Languages::ID_BLACK);
    _p_comboBackground->AddLineText(strTextBt.c_str());
}

LPErrInApp OptionsGfx::OptionsEnd() {
    TRACE("Options End\n");
    _inProgress = false;

    setLanguageInGameSettings();
    setBackgoundTypeInGameSettings();

    DeckType dt;
    dt.SetTypeIndex(_p_comboDeck->GetSelectedIndex());
    _p_GameSettings->DeckTypeVal.CopyFrom(dt);
    _p_GameSettings->MusicEnabled = _p_checkMusic->GetCheckState();
    _p_GameSettings->PlayerName = _p_textInput->GetText();

    if ((_p_GameSettings->MusicEnabled != _prevMusicEnabled) ||
        (_p_GameSettings->DeckTypeVal.GetType() != _prevDeckType) ||
        (_p_GameSettings->BackgroundType != _prevBackgroundType) ||
        (_p_GameSettings->PlayerName != _prevName) ||
        (_p_GameSettings->CurrentLanguage != _prevLangId)) {
        TRACE("Settings are changed\n");
        _p_GameSettings->SetCurrentLang();

        LPErrInApp err = _optDlgt.tc->SettingsChanged(
            _optDlgt.self,
            (_p_GameSettings->BackgroundType != _prevBackgroundType),
            (_p_GameSettings->CurrentLanguage != _prevLangId));
        if (err) {
            return err;
        }
        return _p_GameSettings->SaveSettings();
    }
    return NULL;
}

void OptionsGfx::setLanguageInGameSettings() {
    switch (_p_comboLang->GetSelectedIndex()) {
        case 0:
            _p_GameSettings->CurrentLanguage = Languages::eLangId::LANG_ITA;
            break;
        case 1:
            _p_GameSettings->CurrentLanguage =
                Languages::eLangId::LANG_DIAL_BREDA;
            break;
        case 2:
            _p_GameSettings->CurrentLanguage = Languages::eLangId::LANG_ENG;
            break;
        default:
            break;
    }
}

void OptionsGfx::setBackgoundTypeInGameSettings() {
    switch (_p_comboBackground->GetSelectedIndex()) {
        case 0:
            _p_GameSettings->BackgroundType = BackgroundTypeEnum::Commessaggio;
            break;
        case 1:
            _p_GameSettings->BackgroundType = BackgroundTypeEnum::Mantova;
            break;
        case 2:
            _p_GameSettings->BackgroundType = BackgroundTypeEnum::Black;
            break;
        default:
            break;
    }
}

void OptionsGfx::CheckboxMusicClicked(bool state) {
    if (state) {
        TRACE_DEBUG("Enable music by option \n");
        _p_MusicManager->EnableMusic();
        _p_MusicManager->PlayCurrentMusic();
    } else {
        _p_MusicManager->StopMusic(500);
        _p_MusicManager->DisableMusic();
        TRACE_DEBUG("Disable music by option \n");
    }
}
