#include "HighScore.h"

#include <memory.h>
#include <stdlib.h>

#include "Fading.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "MusicManager.h"
#include "WinTypeGlobal.h"

using namespace std;

static char g_filepath[1024];

HighScore::HighScore() {
    _p_FadeAction = new FadeAction();
    for (int k = 0; k < 10; k++) {
        _scoreInfo[k].Score = 5940 - (k * 250);
        if (k > 1) {
            _scoreInfo[k].Score = _scoreInfo[k].Score - (k - 1) * 50;
        }
        if (k > 2) {
            _scoreInfo[k].Score = _scoreInfo[k].Score - (k - 2) * 20;
        }
        _scoreInfo[k].Name = "Re dal sulitari";
        _scoreInfo[k].NumCard = 40;
    }
}

HighScore::~HighScore() {
    TRACE_DEBUG("HighScore destructor\n");
    delete _p_FadeAction;
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
        _p_ScreenTexture = NULL;
    }
}

LPErrInApp HighScore::Save() {
    LPGameSettings pGameSettings = GameSettings::GetSettings();
    if (pGameSettings->SettingsDir == "" || pGameSettings->GameName == "") {
        return ERR_UTIL::ErrorCreate("User dir for high score is not defined");
    }

    snprintf(g_filepath, sizeof(g_filepath), "%s/%s-score.bin",
             pGameSettings->SettingsDir.c_str(),
             pGameSettings->GameName.c_str());
    TRACE("Save high score file %s\n", g_filepath);

    SDL_IOStream* dst = SDL_IOFromFile(g_filepath, "wb");
    if (dst == 0) {
        return ERR_UTIL::ErrorCreate("Unable to save high score file %s",
                                     g_filepath);
    }

    for (int k = 0; k < NUMOFSCORE; k++) {
        char name[16];
        memset(name, 0, 16);
        memcpy(name, _scoreInfo[k].Name.c_str(), 15);
        int numWritten = SDL_WriteIO(dst, name, 16);
        if (numWritten < 1) {
            return ERR_UTIL::ErrorCreate("SDL_RWwrite name highscore %s\n",
                                         SDL_GetError());
        }
        if (SDL_WriteU16LE(dst, _scoreInfo[k].Score) == 0) {
            return ERR_UTIL::ErrorCreate("SDL_RWwrite score highscore %s\n",
                                         SDL_GetError());
        }
        numWritten = SDL_WriteIO(dst, &_scoreInfo[k].NumCard, 1);
        if (numWritten < 1) {
            return ERR_UTIL::ErrorCreate("SDL_RWwrite numCard highscore %s\n",
                                         SDL_GetError());
        }
    }
    SDL_CloseIO(dst);
    return NULL;
}

LPErrInApp HighScore::SaveScore(int score, int numCard) {
    int j = -1;
    for (int i = 0; i < NUMOFSCORE; i++) {
        if (score > _scoreInfo[i].Score) {
            j = i;
            break;
        }
    }
    if (j > -1) {
        LPGameSettings pGameSettings = GameSettings::GetSettings();
        TRACE("Saving score for user %s\n", pGameSettings->PlayerName.c_str());
        ScoreInfo prev;
        for (int i = j; i < NUMOFSCORE; i++) {
            if (i == j) {
                prev = _scoreInfo[i];
                _scoreInfo[i].Name = pGameSettings->PlayerName;
                _scoreInfo[i].Score = score;
                _scoreInfo[i].NumCard = numCard;
            } else {
                ScoreInfo temp = _scoreInfo[i];
                _scoreInfo[i] = prev;
                prev = temp;
            }
        }
        return Save();
    }
    return NULL;
}

LPErrInApp HighScore::Load() {
    LPGameSettings pGameSettings = GameSettings::GetSettings();

    snprintf(g_filepath, sizeof(g_filepath), "%s/%s-score.bin",
             pGameSettings->SettingsDir.c_str(),
             pGameSettings->GameName.c_str());
    TRACE("Load high score file %s\n", g_filepath);
    SDL_IOStream* src = SDL_IOFromFile(g_filepath, "rb");
    if (src == 0) {
        TRACE("No score file found, ignore it and use default\n");
        return NULL;
    }

    for (int k = 0; k < 10; k++) {
        char name[16];
        uint16_t score = 0;
        uint8_t numCard;
        if (SDL_ReadIO(src, &name, 16) == 0) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWread on highscore file error (file %s): %s\n",
                g_filepath, SDL_GetError());
        }
        SDL_ReadU16LE(src, &score);
        if (SDL_ReadIO(src, &numCard, 1) == 0) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWread on highscore file error (file %s): %s\n",
                g_filepath, SDL_GetError());
        }

        _scoreInfo[k].Name = name;
        _scoreInfo[k].Score = score;
        _scoreInfo[k].NumCard = numCard;
    }
    SDL_CloseIO(src);
    return NULL;
}

LPErrInApp HighScore::HandleIterate(bool& done) {
    SDL_Rect dest;
    if (_state == HighScore::READY_TO_START) {
        return NULL;
    }

    if (_state == HighScore::INIT) {
        if (_p_ScreenTexture != NULL) {
            SDL_DestroyTexture(_p_ScreenTexture);
        }
        _p_ScreenTexture =
            SDL_CreateTextureFromSurface(_p_sdlRenderer, _p_surfScreen);
        if (!_ignoreMouseEvent) {
            _p_FadeAction->Fade(_p_surfScreen, _p_surfScreen, 2, 1,
                                _p_sdlRenderer, NULL);
        } else {
            _p_FadeAction->InstantFade(_p_surfScreen);
        }
        _p_MusicManager->PlayMusic(MusicManager::MUSIC_CREDITS_SND,
                                   MusicManager::LOOP_ON);
        _state = HighScore::IN_PROGRESS;
        return NULL;
    }

    if (_state == HighScore::IN_PROGRESS) {
        Uint64 last_time = SDL_GetTicks();
        dest.x = (_p_surfScreen->w - _p_SurfTitle->w) / 2;
        dest.y = 0;
        dest.w = _p_SurfTitle->w;
        dest.h = _p_SurfTitle->h;

        SDL_BlitSurface(_p_SurfTitle, NULL, _p_surfScreen, &dest);
        int ax = 70;
        int bx = 300;
        int cx = 100;
        int dx = 30;
        int ex = 50;
        if (_p_GameSettings->NeedScreenMagnify()) {
            ax = 100;
            bx = 550;
            cx = 180;
            dx = 50;
            ex = 100;
        }
        int xIni = (_p_surfScreen->w - (ax + bx + cx + dx + ex)) / 2;
        int yIni = 200;
        int xOff, yOff;
        char buff[256];
        xOff = 0;
        yOff = 0;
        TTF_Font* pFont = _p_GameSettings->GetFontAriblk();
        TTF_Font* pFont2 = _p_GameSettings->GetFontVera();
        LPLanguages pLanguages = _p_GameSettings->GetLanguageMan();

        xOff += ax;
        SDL_Color txtColor = GFX_UTIL_COLOR::Gray;
        GFX_UTIL::DrawString(_p_surfScreen,
                             pLanguages->GetCStringId(Languages::NAME),
                             xIni + xOff, yIni + yOff, txtColor, pFont2);
        xOff += bx;
        GFX_UTIL::DrawString(_p_surfScreen,
                             pLanguages->GetCStringId(Languages::POINTS),
                             xIni + xOff, yIni + yOff, txtColor, pFont2);
        xOff += cx;
        GFX_UTIL::DrawString(_p_surfScreen,
                             pLanguages->GetCStringId(Languages::CARDDECK),
                             xIni + xOff, yIni + yOff, txtColor, pFont2);
        yOff += dx;
        xOff = 0;

        for (int i = 0; i < NUMOFSCORE; i++) {
            snprintf(buff, sizeof(buff), "#%d", i + 1);
            if (i == 0) {
                txtColor = GFX_UTIL_COLOR::Red;
            } else if (i == 1) {
                txtColor = GFX_UTIL_COLOR::Orange;
            } else if (i == 2) {
                txtColor = GFX_UTIL_COLOR::Green;
            } else {
                txtColor = GFX_UTIL_COLOR::White;
            }
            GFX_UTIL::DrawString(_p_surfScreen, buff, xIni + xOff, yIni + yOff,
                                 txtColor, pFont);
            xOff += ax;
            GFX_UTIL::DrawString(_p_surfScreen, _scoreInfo[i].Name.c_str(),
                                 xIni + xOff, yIni + yOff, txtColor, pFont);
            xOff += bx;
            snprintf(buff, sizeof(buff), "%d", _scoreInfo[i].Score);
            GFX_UTIL::DrawString(_p_surfScreen, buff, xIni + xOff, yIni + yOff,
                                 txtColor, pFont);
            xOff += cx;
            snprintf(buff, sizeof(buff), "%d", _scoreInfo[i].NumCard);
            GFX_UTIL::DrawString(_p_surfScreen, buff, xIni + xOff, yIni + yOff,
                                 txtColor, pFont);
            yOff += ex;
            xOff = 0;
        }

        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_surfScreen->pixels,
                          _p_surfScreen->pitch);
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);

        Uint64 now_time = SDL_GetTicks();
        if (now_time < last_time + (1000 / 20)) {
            SDL_Delay(last_time + (1000 / 20) - now_time);
        }
        uint32_t elapsed_sec = (now_time / 1000) - (_start_time / 1000);
        if (elapsed_sec > 30) {
            TRACE_DEBUG("after 30 sec, time to exit from high score\n");
            _state = HighScore::DONE;
        }

        return NULL;
    }

    if (_state == HighScore::DONE) {
        _p_FadeAction->Fade(_p_surfScreen, _p_surfScreen, 1, 1, _p_sdlRenderer,
                            NULL);
        _state = HighScore::TERMINATED;
        return NULL;
    }

    if (_state == HighScore::TERMINATED) {
        done = true;
        _state = HighScore::READY_TO_START;
        return NULL;
    }

    return NULL;
}

LPErrInApp HighScore::HandleEvent(SDL_Event* pEvent) {
    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_ESCAPE) {
            _state = HighScore::DONE;
        }
    }
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        _state = HighScore::DONE;
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        _state = HighScore::DONE;
    }
    return NULL;
}

LPErrInApp HighScore::Show(SDL_Surface* p_surf_screen, SDL_Surface* pSurfTitle,
                           SDL_Renderer* psdlRenderer) {
    _p_sdlRenderer = psdlRenderer;
    _p_surfScreen = p_surf_screen;
    _p_SurfTitle = pSurfTitle;
    _start_time = SDL_GetTicks();

    _p_GameSettings = GameSettings::GetSettings();
    _p_MusicManager = _p_GameSettings->GetMusicManager();
    _ignoreMouseEvent =
        _p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse;
    _state = HighScore::INIT;
    return NULL;
}
