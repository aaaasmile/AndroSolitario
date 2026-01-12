#include "KeyboardGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

KeyboardGfx::KeyboardGfx() {
    _p_keyboardSurface = NULL;
    _p_fontText = NULL;
    _visibleState = VisbleState::VISIBLE;
    _fncbKeyboardEvent.self = NULL;
    _fncbKeyboardEvent.tc = NULL;
    _isShifted = true; // Start with capitalized
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
    if (btID == ID_SHIFT) {
        _isShifted = !_isShifted;
        RefreshLabels();
        return;
    }

    if (_fncbKeyboardEvent.tc && _fncbKeyboardEvent.tc->ClickKey) {
        char text[2] = {0, 0};
        if (btID == ID_BACKSPACE) {
            text[0] = '\b';
        } else if (btID == ID_RETURN) {
            text[0] = '\n';
        } else if (btID == ID_SPACE) {
            text[0] = ' ';
        } else if (btID >= 'A' && btID <= 'Z') {
            text[0] = _isShifted ? (char)btID : (char)(btID + 32);
        } else {
            text[0] = (char)btID;
        }
        _fncbKeyboardEvent.tc->ClickKey(_fncbKeyboardEvent.self, text);
    }
}

void KeyboardGfx::RefreshLabels() {
    int btnIdx = 0;
    const char* layout[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    for (int r = 0; r < 3; ++r) {
        int keysInRow = strlen(layout[r]);
        for (int k = 0; k < keysInRow; ++k) {
            char c = layout[r][k];
            if (!_isShifted) {
                c += 32;
            }
            char text[2] = {c, 0};
            _buttons[btnIdx]->SetButtonText(text);
            btnIdx++;
        }
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
                                   NULL, 153, 242, 51));

    SDL_SetSurfaceBlendMode(_p_keyboardSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_keyboardSurface, 150);

    // Initialize buttons
    const char* layout[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    int rowCount = 4;
    int margin = 5;
    int btnGap = 2;

    int maxKeysInRow = 10;
    int btnW =
        (_rctCtrl.w - (margin * 2) - (maxKeysInRow - 1) * btnGap) / maxKeysInRow;
    int btnH = (_rctCtrl.h - (margin * 2) - (rowCount - 1) * btnGap) / rowCount;

    ClickCb clickCb = prepClickCb();
    for (int r = 0; r < 3; ++r) {
        int keysInRow = strlen(layout[r]);
        int rowW = keysInRow * btnW + (keysInRow - 1) * btnGap;
        int startX = _rctCtrl.x + (_rctCtrl.w - rowW) / 2;
        int startY = _rctCtrl.y + margin + r * (btnH + btnGap);

        for (int k = 0; k < keysInRow; ++k) {
            SDL_Rect btnRect = {startX + k * (btnW + btnGap), startY, btnW,
                                btnH};
            ButtonGfx* pBtn = new ButtonGfx();
            pBtn->Initialize(&btnRect, pScreen, pFont, (int)layout[r][k],
                             clickCb);
            pBtn->SetVisibleState(ButtonGfx::VisbleState::VISIBLE);
            std::string s = std::string(&layout[r][k], 1);
            pBtn->SetButtonText(s.c_str());
            _buttons.push_back(pBtn);
        }
    }

    // 4th row: Shift, Space, Backspace, Return
    int startY4 = _rctCtrl.y + margin + 3 * (btnH + btnGap);
    int startX4 = _rctCtrl.x + margin;

    int wShift = (int)(1.5 * btnW + 0.5 * btnGap);
    int wSpace = (int)(5 * btnW + 4 * btnGap);
    int wBS = (int)(1.5 * btnW + 0.5 * btnGap);
    int wRet = (int)(2 * btnW + 1 * btnGap);

    struct SpecialKey {
        int id;
        const char* label;
        int width;
    } specialKeys[] = {
        {ID_SHIFT, "Shift", wShift},
        {ID_SPACE, "Space", wSpace},
        {ID_BACKSPACE, "BS", wBS},
        {ID_RETURN, "Ret", wRet}
    };

    int currentX = startX4;
    for (int i = 0; i < 4; ++i) {
        SDL_Rect btnRect = {currentX, startY4, specialKeys[i].width, btnH};
        ButtonGfx* pBtn = new ButtonGfx();
        pBtn->Initialize(&btnRect, pScreen, pFont, specialKeys[i].id, clickCb);
        pBtn->SetVisibleState(ButtonGfx::VisbleState::VISIBLE);
        pBtn->SetButtonText(specialKeys[i].label);
        _buttons.push_back(pBtn);
        currentX += specialKeys[i].width + btnGap;
    }

    RefreshLabels();
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