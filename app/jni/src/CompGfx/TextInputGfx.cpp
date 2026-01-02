#include "TextInputGfx.h"

#include <algorithm>

#include "Config.h"
#include "GameSettings.h"
#include "GfxUtil.h"

#if HASTOUCH
static bool IsPointInsideCtrl(const SDL_Rect& rct, const SDL_Point& pt) {
    if (pt.x >= rct.x && pt.x <= rct.x + rct.w && pt.y >= rct.y &&
        pt.y <= rct.y + rct.h) {
        return true;
    }
    return false;
}
#endif

TextInputGfx::TextInputGfx() {
    _visibleState = INVISIBLE;
    _lastBlinkTime = 0;
    _hasFocus = false;
    _cursorVisible = false;
    _p_Window = NULL;
}

TextInputGfx::~TextInputGfx() {
    if (_p_ctrlSurface) {
        SDL_DestroySurface(_p_ctrlSurface);
        _p_ctrlSurface = NULL;
    }
}

void TextInputGfx::Initialize(SDL_Rect* pRect, SDL_Surface* pScreen,
                              TTF_Font* pFont, SDL_Window* pWindow) {
    _rctCtrl = *pRect;
    _p_Window = pWindow;

    _p_ctrlSurface =
        GFX_UTIL::SDL_CreateRGBSurface(_rctCtrl.w, _rctCtrl.h, 32, 0, 0, 0, 0);

    SDL_FillSurfaceRect(_p_ctrlSurface, NULL,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(pScreen->format),
                                   NULL, 12, 12, 12));

    SDL_SetSurfaceBlendMode(_p_ctrlSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(_p_ctrlSurface, 127);
    _p_fontText = pFont;
}

void TextInputGfx::HandleEvent(SDL_Event* pEvent, const SDL_Point& targetPos) {
#if HASMOUSE
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        TRACE_DEBUG("[TextInput - event] SDL_EVENT_MOUSE_BUTTON_DOWN \n");
        if (pGameSettings->InputType == InputTypeEnum::TouchWithoutMouse) {
            return;
        }
        int mouseX = targetPos.x;
        int mouseY = targetPos.y;

        _hasFocus =
            (mouseX >= _rctCtrl.x && mouseX <= _rctCtrl.x + _rctCtrl.w &&
             mouseY >= _rctCtrl.y && mouseY <= _rctCtrl.y + _rctCtrl.h);

        if (_hasFocus) {
            const char* val = SDL_GetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT);
            TRACE_DEBUG(
                "[TextInput - event] calling SDL_StartTextInput for: %s \n",
                val);
            SDL_StartTextInput(_p_Window);
        } else {
            SDL_StopTextInput(_p_Window);
        }
    }
#endif
    if (_hasFocus && pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_BACKSPACE && !_text.empty()) {
            _text.pop_back();
        } else if (pEvent->key.key == SDLK_RETURN ||
                   pEvent->key.key == SDLK_TAB) {
            _hasFocus = false;
            SDL_StopTextInput(_p_Window);
        }
    }
    if (_hasFocus && pEvent->type == SDL_EVENT_TEXT_INPUT) {
        if (_text.length() < 20) {
            std::string newText = pEvent->text.text;
            if (std::all_of(newText.begin(), newText.end(), [](char c) {
                    return std::isalnum(c) || c == ' ' || c == '-';
                })) {
                _text += newText;
            }
        }
    }
    if (_hasFocus && pEvent->type == SDL_EVENT_TEXT_EDITING) {
        TRACE_DEBUG("[HandleEvent - SDL_EVENT_TEXT_EDITING] : %s \n", pEvent->text.text);
    }

#if HASTOUCH
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        SDL_Point pt;
        pGameSettings->GetTouchPoint(pEvent->tfinger, &pt);
        _hasFocus = IsPointInsideCtrl(_rctCtrl, pt);
        if (_hasFocus) {
            SDL_StartTextInput(_p_Window);
        } else {
            SDL_StopTextInput(_p_Window);
        }
    }
#endif
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
    } else {
        LPGameSettings pGameSettings = GameSettings::GetSettings();
        Languages* pLang = pGameSettings->GetLanguageMan();
        int tx, ty;
        const char* namePlaceHolder =
            pLang->GetCStringId(Languages::PLEASE_ENTER_NAME);
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