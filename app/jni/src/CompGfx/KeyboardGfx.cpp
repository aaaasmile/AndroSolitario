#include "KeyboardGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

KeyboardGfx::KeyboardGfx() {
    _p_keyboardSurface = NULL;
    _p_fontText = NULL;
    _visibleState = VisbleState::VISIBLE;
    _fncbKeyboardEvent.self = NULL;
    _fncbKeyboardEvent.tc = NULL;
}

KeyboardGfx::~KeyboardGfx() {
    for (size_t i = 0; i < _buttons.size(); ++i) {
        delete _buttons[i];
    }
    _buttons.clear();

    if (_p_keyboardSurface) {
        SDL_DestroySurface(_p_keyboardSurface);
        _p_keyboardSurface = NULL;
    }
}

void fncBind_OnButtonClick(void* self, int btID) {
    KeyboardGfx* pKeyboard = (KeyboardGfx*)self;
    pKeyboard->OnButtonClickImpl(btID);
}

ClickCb KeyboardGfx::prepClickCb() {
    // Use only static otherwise you loose it
    static VClickCb const tc = {.Click = (&fncBind_OnButtonClick)};
    return (ClickCb){.tc = &tc, .self = this};
}


void KeyboardGfx::OnButtonClickImpl(int btID) {
    if (_fncbKeyboardEvent.tc && _fncbKeyboardEvent.tc->ClickKey) {
        char text[2] = {(char)btID, 0};
        _fncbKeyboardEvent.tc->ClickKey(_fncbKeyboardEvent.self, text);
    }
}

void KeyboardGfx::Show(SDL_Rect* pRect, SDL_Surface* pScreen, TTF_Font* pFont,
                       ClickKeyboardCb& fncbClickEvent) {
    _fncbKeyboardEvent = fncbClickEvent;
    _p_fontText = pFont;
    _rctCtrl = *pRect;
    
    for (size_t i = 0; i < _buttons.size(); ++i) {
        delete _buttons[i];
    }
    _buttons.clear();

    _p_keyboardSurface =
        GFX_UTIL::SDL_CreateRGBSurface(_rctCtrl.w, _rctCtrl.h, 32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_keyboardSurface, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 255, 0, 0));

    SDL_SetSurfaceBlendMode(_p_keyboardSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_keyboardSurface, 127);

    // Initialize buttons
    const char* layout[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    int rowCount = 3;
    int margin = 5;
    int btnGap = 2;

    int maxKeysInRow = 10;
    int btnW = (_rctCtrl.w - (margin * 2) - (maxKeysInRow - 1) * btnGap) / maxKeysInRow;
    int btnH = (_rctCtrl.h - (margin * 2) - (rowCount - 1) * btnGap) / rowCount;
    
    ClickCb clickCb = prepClickCb();
    for (int r = 0; r < rowCount; ++r) {
        int keysInRow = strlen(layout[r]);
        int rowW = keysInRow * btnW + (keysInRow - 1) * btnGap;
        int startX = _rctCtrl.x + (_rctCtrl.w - rowW) / 2;
        int startY = _rctCtrl.y + margin + r * (btnH + btnGap);

        for (int k = 0; k < keysInRow; ++k) {
            SDL_Rect btnRect = {startX + k * (btnW + btnGap), startY, btnW, btnH};
            ButtonGfx* pBtn = new ButtonGfx();
            pBtn->Initialize(&btnRect, pScreen, pFont, (int)layout[r][k], clickCb);
            pBtn->SetVisibleState(ButtonGfx::VisbleState::VISIBLE);
            std::string s = std::string(&layout[r][k], 1);
            pBtn->SetButtonText(s.c_str());
            _buttons.push_back(pBtn);
        }
    }
}

void KeyboardGfx::DrawCtrl(SDL_Surface* pScreen) {
    if (_visibleState == VisbleState::INVISIBLE) {
        return;
    }
    // Draw background
    GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctCtrl.w, _rctCtrl.h,
                                 _rctCtrl.x, _rctCtrl.y, _p_keyboardSurface);

    // Draw border
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 1, _rctCtrl.y - 1,
                       _rctCtrl.x + _rctCtrl.w + 1, _rctCtrl.y + _rctCtrl.h + 1,
                       GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 2, _rctCtrl.y - 2,
                       _rctCtrl.x + _rctCtrl.w + 2, _rctCtrl.y + _rctCtrl.h + 2,
                       GFX_UTIL_COLOR::Black);

    for (size_t i = 0; i < _buttons.size(); ++i) {
        _buttons[i]->DrawButton(pScreen);
    }
}

void KeyboardGfx::HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos) {
    if (_visibleState == VisbleState::INVISIBLE) {
        return;
    }

    for (size_t i = 0; i < _buttons.size(); ++i) {
        switch (pEvent->type) {
            case SDL_EVENT_MOUSE_MOTION:
                _buttons[i]->MouseMove(pEvent, targetPos);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                _buttons[i]->MouseDown(pEvent, targetPos);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                _buttons[i]->MouseUp(pEvent, targetPos);
                break;
            case SDL_EVENT_FINGER_DOWN:
                _buttons[i]->FingerDown(pEvent);
                break;
        }
    }
}