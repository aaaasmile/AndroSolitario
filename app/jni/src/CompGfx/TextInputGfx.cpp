#include "TextInputGfx.h"

#include "GameSettings.h"
#include "GfxUtil.h"

TextInputGfx::TextInputGfx() {
    _visibleState = INVISIBLE;
    _lastBlinkTime = 0;
}

TextInputGfx::~TextInputGfx() {
    if (_p_ctrlSurface) {
        SDL_DestroySurface(_p_ctrlSurface);
        _p_ctrlSurface = NULL;
    }
}

void TextInputGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                              TTF_Font* pFont) {
    _rctCtrl = *pRect;

    _p_ctrlSurface =
        GFX_UTIL::SDL_CreateRGBSurface(_rctCtrl.w, _rctCtrl.h, 32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_ctrlSurface, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 12, 12, 12));

    SDL_SetSurfaceBlendMode(_p_ctrlSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_ctrlSurface, 127);
    _p_fontText = pFont;
}

void TextInputGfx::HandleEvent(const SDL_Event& event, SDL_Window* pWindow) {
    LPGameSettings pGameSettings = GameSettings::GetSettings();

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (pGameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
            return;
        }
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        _hasFocus =
            (mouseX >= _rctCtrl.x && mouseX <= _rctCtrl.x + _rctCtrl.w &&
             mouseY >= _rctCtrl.y && mouseY <= _rctCtrl.y + _rctCtrl.h);

        if (_hasFocus) {
            SDL_StartTextInput(pWindow);
        } else {
            SDL_StopTextInput(pWindow);
        }
    } else if (_hasFocus && event.type == SDL_EVENT_TEXT_INPUT) {
        _text += event.text.text;
    } else if (_hasFocus && event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_BACKSPACE && !_text.empty()) {
            _text.pop_back();
        } else if (event.key.key == SDLK_RETURN) {
            _hasFocus = false;
            SDL_StopTextInput(pWindow);
        }
    } else if (_hasFocus && event.type == SDL_EVENT_TEXT_INPUT) {
        _text += event.text.text;
    }
}

void TextInputGfx::DrawCtrl(SDL_Surface* pScreen) {
    // Draw background
    GFX_UTIL::DrawStaticSpriteEx(pScreen, 0, 0, _rctCtrl.w, _rctCtrl.h,
                                 _rctCtrl.x, _rctCtrl.y, _p_ctrlSurface);

    // Draw border
    SDL_Color colorFocus;
    if (_hasFocus) {
        colorFocus = GFX_UTIL_COLOR::Orange;
    } else {
        colorFocus = GFX_UTIL_COLOR::White;
    }
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 1, _rctCtrl.y - 1,
                       _rctCtrl.x + _rctCtrl.w + 1, _rctCtrl.y + _rctCtrl.h + 1,
                       GFX_UTIL_COLOR::Gray);
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x - 2, _rctCtrl.y - 2,
                       _rctCtrl.x + _rctCtrl.w + 2, _rctCtrl.y + _rctCtrl.h + 2,
                       GFX_UTIL_COLOR::Black);
    GFX_UTIL::DrawRect(pScreen, _rctCtrl.x, _rctCtrl.y, _rctCtrl.x + _rctCtrl.w,
                       _rctCtrl.y + _rctCtrl.h, colorFocus);

    // Draw text
    int offsetX = 5;
    if (!_text.empty()) {
        int tx, ty;
        TTF_GetStringSize(_p_fontText, _text.c_str(), 0, &tx, &ty);
        int offsetY = (_rctCtrl.h - ty) / 2;

        GFX_UTIL::DrawString(pScreen, _text.c_str(), _rctCtrl.x + offsetX,
                             _rctCtrl.y + offsetY, GFX_UTIL_COLOR::White,
                             _p_fontText);
    }else{
        int tx, ty;
        const char* namePlaceHolder = "Please enter the name";
        TTF_GetStringSize(_p_fontText, namePlaceHolder, 0, &tx, &ty);
        int offsetY = (_rctCtrl.h - ty) / 2;

        GFX_UTIL::DrawString(pScreen, namePlaceHolder, _rctCtrl.x + offsetX,
                             _rctCtrl.y + offsetY, GFX_UTIL_COLOR::Gray,
                             _p_fontText);
    }

    // Draw cursor
    if (_hasFocus && _cursorVisible) {
        int cursorX = _rctCtrl.x + offsetX;
        if (!_text.empty()) {
            // Calculate cursor position based on text width
            int textWidth, textHeight;
            TTF_GetStringSize(_p_fontText, _text.c_str(), 0, &textWidth,
                              &textHeight);
            cursorX += textWidth;
        }

        SDL_Rect cursor = {cursorX, _rctCtrl.y + 5, 2, _rctCtrl.h - 10};
        Uint32 colorCursor = SDL_MapRGB(
            SDL_GetPixelFormatDetails(pScreen->format), NULL, 250, 250, 250);

        GFX_UTIL::FillRect(pScreen, cursor.x, cursor.y, cursor.w, cursor.h,
                           colorCursor);
    }
}

void TextInputGfx::Update() {
    if (_hasFocus && SDL_GetTicks() - _lastBlinkTime > 500) {
        _cursorVisible = !_cursorVisible;
        _lastBlinkTime = SDL_GetTicks();
    }
}