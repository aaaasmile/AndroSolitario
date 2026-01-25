#include "HighScore.h"

#include <memory.h>
#include <stdlib.h>

#include "Config.h"
#include "Fading.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "MusicManager.h"
#include "TypeGlobal.h"

#if PLATFORM_EMS
#include <emscripten.h>

#include <iostream>
#include <sstream>
#include <vector>
#else
static char g_filepath[1024];
#endif

using namespace std;

HighScore::HighScore() {
    _p_FadeAction = new FadeAction();
    _p_surfScreen = NULL;
    _p_SurfTitle = NULL;
    _fnUpdateScreen.tc = NULL;
    _fnUpdateScreen.self = NULL;
    _p_GameSettings = NULL;
    _p_MusicManager = NULL;
    _lastUpTimestamp = 0;
    _state = HighScore::READY_TO_START;

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
}

#if PLATFORM_EMS
static std::string loadGameHighScore(const char* key,
                                     const char* defaultValue) {
    char js_command[256];
    snprintf(js_command, sizeof(js_command),
             "localStorage.getItem('high_%s') || '%s'", key, defaultValue);
    const char* result = emscripten_run_script_string(js_command);
    return result != NULL ? std::string(result) : std::string(defaultValue);
}

static void saveGameHighScore(const char* key, const char* value) {
    char js_cmd[256];
    snprintf(js_cmd, sizeof(js_cmd), "localStorage.setItem('high_%s', '%s')",
             key, value);
    emscripten_run_script(js_cmd);
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
#endif

LPErrInApp HighScore::Save() {
#if PLATFORM_EMS
    for (int k = 0; k < NUMOFSCORE; k++) {
        std::string kstr = std::to_string(k);
        std::string val = _scoreInfo[k].Name + ":" +
                          std::to_string(_scoreInfo[k].Score) + ":" +
                          std::to_string(_scoreInfo[k].NumCard);
        saveGameHighScore(kstr.c_str(), val.c_str());
    }
#else
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
#endif
    return NULL;
}

LPErrInApp HighScore::SaveScore(int64_t score, int numCard) {
    if (score < 0 || score > 1000000) {
        return NULL;
    }
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
#if PLATFORM_EMS
    for (int k = 0; k < NUMOFSCORE; k++) {
        std::string kstr = std::to_string(k);
        std::string result = loadGameHighScore(kstr.c_str(), "");
        if (result == "") {
            TRACE_DEBUG("No high score in storage found, use defaults \n");
            return NULL;
        }
        int numCard = 40;
        std::string name = "";
        int score = 0;
        std::vector<std::string> parts = splitString(result, ':');
        if (parts.size() >= 2) {
            name = parts[0];
            score = std::stoi(parts[1]);
            if (parts.size() >= 3) {
                numCard = std::stoi(parts[2]);
            }
        }
        _scoreInfo[k].Name = name;
        _scoreInfo[k].Score = score;
        _scoreInfo[k].NumCard = numCard;
    }
#else
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
#endif
    return NULL;
}

LPErrInApp HighScore::HandleEvent(SDL_Event* pEvent) {
    if (_state != HighScore::IN_PROGRESS &&
        _state != HighScore::IN_PROGRESS_WAIT) {
        return NULL;
    }
    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_ESCAPE || pEvent->key.key == SDLK_RETURN) {
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

LPErrInApp HighScore::HandleIterate(bool& done) {
    SDL_Rect src, dest;
    if (_state == HighScore::READY_TO_START) {
        return NULL;
    }
    if (_state == HighScore::WAIT_FOR_FADING) {
        if (_p_FadeAction->IsInProgress()) {
            _p_FadeAction->Iterate();
            return NULL;
        }
        TRACE("HighScore - fade end, next state is %d \n", _stateAfter);
        if (_state == _stateAfter) {
            return ERR_UTIL::ErrorCreate(
                "Next state could not be WAIT_FOR_FADING\n");
        }
        _state = _stateAfter;
        return NULL;
    }

    if (_state == HighScore::INIT) {
        TRACE("HighScore Init \n");
        if (!_ignoreMouseEvent) {
            TRACE("HighScore Init - fade start \n");
            _p_FadeAction->Fade(_p_surfScreen, _p_surfScreen, 2, true,
                                _fnUpdateScreen, NULL);
            _state = HighScore::WAIT_FOR_FADING;
            _stateAfter = HighScore::IN_PROGRESS;
        } else {
            _p_FadeAction->InstantFade(_p_surfScreen);
            _state = HighScore::IN_PROGRESS;
        }
        _p_MusicManager->PlayMusic(MusicManager::MUSIC_CREDITS_SND,
                                   MusicManager::LOOP_ON);
        return NULL;
    }

    if (_state == HighScore::IN_PROGRESS) {
        dest.x = (_p_surfScreen->w - _p_SurfTitle->w) / 2;
        dest.y = 0;
        dest.w = _p_SurfTitle->w;
        dest.h = _p_SurfTitle->h;

        SDL_BlitSurface(_p_SurfTitle, NULL, _p_surfScreen, &dest);

        src.x = 0;
        src.y = (_p_SurfTitle->h) + 2;
        src.w = _p_surfScreen->w;
        src.h = _p_surfScreen->h - (_p_SurfTitle->h);

        dest.x = 0;
        dest.y = (_p_SurfTitle->h);
        dest.w = src.w;
        dest.h = src.h;

        SDL_BlitSurface(_p_surfScreen, &src, _p_surfScreen, &dest);

        dest.x = 0;
        dest.y = (_p_surfScreen->h) - 2;
        dest.w = _p_surfScreen->w;
        dest.h = 2;

        SDL_FillSurfaceRect(
            _p_surfScreen, &dest,
            SDL_MapRGB(SDL_GetPixelFormatDetails(_p_surfScreen->format), NULL,
                       0, 0, 0));

        int ax = 70;
        int bx = 300;
        int cx = 100;
        int dx = 30;
        int ex = 50;
        int xIni = (_p_surfScreen->w - (ax + bx + cx + dx + ex)) / 2;
        int yIni = 200;
        int xOff, yOff;
        char buff[256];
        xOff = 0;
        yOff = 0;
        TTF_Font* pFont = _p_GameSettings->GetFontDjvBig();
        TTF_Font* pFont2 = _p_GameSettings->GetFontDjvSmall();
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

        _state = HighScore::IN_PROGRESS_WAIT;
        // TRACE_DEBUG("[HandleIterate] next state is IN_PROGRESS_WAIT \n");
    }
    if (_state == HighScore::IN_PROGRESS_WAIT) {
        Uint64 now_time = SDL_GetTicks();
        if (_lastUpTimestamp + 30 > now_time) {
            // TRACE_DEBUG("Ignore iteration because too fast \n");
            return NULL;
        }
        _lastUpTimestamp = now_time;

        (_fnUpdateScreen.tc)->UpdateScreen(_fnUpdateScreen.self, _p_surfScreen);

        uint32_t elapsed_sec = (now_time / 1000) - (_start_time / 1000);
        if (elapsed_sec > 20) {
            TRACE_DEBUG("after 20 sec, time to exit from high score\n");
            _state = HighScore::DONE;
            return NULL;
        }

        return NULL;
    }

    if (_state == HighScore::DONE) {
        TRACE("HighScore done \n");
        _p_FadeAction->Fade(_p_surfScreen, _p_surfScreen, 1, true,
                            _fnUpdateScreen, NULL);
        _state = HighScore::WAIT_FOR_FADING;
        _stateAfter = HighScore::TERMINATED;
        return NULL;
    }

    if (_state == HighScore::TERMINATED) {
        TRACE("HighScore terminated \n");
        done = true;
        _state = HighScore::READY_TO_START;
        return NULL;
    }

    return NULL;
}

LPErrInApp HighScore::Show(SDL_Surface* p_surf_screen, SDL_Surface* pSurfTitle,
                           UpdateScreenCb& fnUpdateScreen) {
    TRACE("HighScore Show \n");
    _fnUpdateScreen = fnUpdateScreen;
    _p_surfScreen = p_surf_screen;
    _p_SurfTitle = pSurfTitle;
    _start_time = SDL_GetTicks();
    _lastUpTimestamp = _start_time;

    _p_GameSettings = GameSettings::GetSettings();
    _p_MusicManager = _p_GameSettings->GetMusicManager();
    _ignoreMouseEvent =
        _p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse;
    _state = HighScore::INIT;
    return NULL;
}
