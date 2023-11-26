#include "HighScore.h"

#include <memory.h>
#include <stdlib.h>

#include "Fading.h"
#include "GameSettings.h"
#include "GfxUtil.h"
#include "MusicManager.h"
#include "WinTypeGlobal.h"

using namespace std;

static const char* g_lpszScore = "solitario_score.bin";
static char g_filepath[1024];

HighScore::HighScore() {
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

LPErrInApp HighScore::Save() {
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    
    snprintf(g_filepath, sizeof(g_filepath), "%s/%s",
             pGameSettings->SettingsDir.c_str(), g_lpszScore);
    TRACE("Save high score file %s\n", g_filepath);

    SDL_RWops* dst = SDL_RWFromFile(g_filepath, "wb");
    if (dst == 0) {
        return ERR_UTIL::ErrorCreate("Unable to save high score file %s",
                                     g_filepath);
    }

    for (int k = 0; k < NUMOFSCORE; k++) {
        char name[16];
        memset(name, 0, 16);
        memcpy(name, _scoreInfo[k].Name.c_str(), 15);
        int numWritten = SDL_RWwrite(dst, name, 16, 1);
        if (numWritten < 1) {
            return ERR_UTIL::ErrorCreate("SDL_RWwrite name highscore %s\n",
                                         SDL_GetError());
        }
        if (SDL_WriteLE16(dst, _scoreInfo[k].Score) == 0) {
            return ERR_UTIL::ErrorCreate("SDL_RWwrite score highscore %s\n",
                                         SDL_GetError());
        }
        numWritten = SDL_RWwrite(dst, &_scoreInfo[k].NumCard, 1, 1);
        if (numWritten < 1) {
            return ERR_UTIL::ErrorCreate("SDL_RWwrite numCard highscore %s\n",
                                         SDL_GetError());
        }
    }
    SDL_RWclose(dst);
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
        char bufName[256];
        sprintf(bufName, "%s", getenv("USERNAME"));
        
        ScoreInfo prev;
        for (int i = j; i < NUMOFSCORE; i++) {
            if (i == j) {
                prev = _scoreInfo[i];
                _scoreInfo[i].Name = bufName;
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
    LPGameSettings pGameSettings = GAMESET::GetSettings();
    
    snprintf(g_filepath, sizeof(g_filepath), "%s/%s",
             pGameSettings->SettingsDir.c_str(), g_lpszScore);
    TRACE("Load high score file %s\n", g_filepath);
    SDL_RWops* src = SDL_RWFromFile(g_filepath, "rb");
    if (src == 0) {
        TRACE("No score file found, ignore it and use default\n");
        return NULL;
    }

    for (int k = 0; k < 10; k++) {
        char name[16];
        uint16_t score = 0;
        uint8_t numCard;
        if (SDL_RWread(src, name, 16, 1) == 0) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWread on highscore file error (file %s): %s\n", g_filepath,
                SDL_GetError());
        }
        score = SDL_ReadLE16(src);
        if (SDL_RWread(src, &numCard, 1, 1) == 0) {
            return ERR_UTIL::ErrorCreate(
                "SDL_RWread on highscore file error (file %s): %s\n", g_filepath,
                SDL_GetError());
        }

        _scoreInfo[k].Name = name;
        _scoreInfo[k].Score = score;
        _scoreInfo[k].NumCard = numCard;
    }
    SDL_RWclose(src);

    return NULL;
}

LPErrInApp HighScore::Show(SDL_Surface* p_surf_screen, SDL_Surface* pSurfTitle,
                           SDL_Renderer* psdlRenderer,
                           MusicManager* pMusicManager, TTF_Font* pFont,
                           TTF_Font* pFont2, Languages* pLanguages) {
    bool done;
    SDL_Rect dest;
    SDL_Event event;
    Uint32 last_time, now_time;
    SDL_Keycode key;

    SDL_Texture* pScreenTexture =
        SDL_CreateTextureFromSurface(psdlRenderer, p_surf_screen);

    fade(p_surf_screen, p_surf_screen, 2, 1, psdlRenderer, NULL);
    if (pMusicManager != NULL) {
        pMusicManager->PlayMusic(MusicManager::MUSIC_GAME_SND,
                                 MusicManager::LOOP_ON);
    }

    dest.x = (p_surf_screen->w - pSurfTitle->w) / 2;
    dest.y = 0;
    dest.w = pSurfTitle->w;
    dest.h = pSurfTitle->h;

    SDL_BlitSurface(pSurfTitle, NULL, p_surf_screen, &dest);

    done = false;

    do {
        last_time = SDL_GetTicks();

        while (SDL_PollEvent(&event) > 0) {
            if (event.type == SDL_QUIT) {
                done = true;
            } else if (event.type == SDL_KEYDOWN) {
                key = event.key.keysym.sym;

                if (key == SDLK_ESCAPE) {
                    done = true;
                }
            }
        }
        int xIni = (p_surf_screen->w - (70 + 300 + 100 + 30 + 50)) / 2;
        int yIni = 200;
        int xOff, yOff;
        char buff[256];
        xOff = 0;
        yOff = 0;

        xOff += 70;
        SDL_Color txtColor = GFX_UTIL_COLOR::Gray;
        GFX_UTIL::DrawString(p_surf_screen,
                             pLanguages->GetCStringId(Languages::NAME),
                             xIni + xOff, yIni + yOff, txtColor, pFont2);
        xOff += 300;
        GFX_UTIL::DrawString(p_surf_screen,
                             pLanguages->GetCStringId(Languages::POINTS),
                             xIni + xOff, yIni + yOff, txtColor, pFont2);
        xOff += 100;
        GFX_UTIL::DrawString(p_surf_screen,
                             pLanguages->GetCStringId(Languages::CARDDECK),
                             xIni + xOff, yIni + yOff, txtColor, pFont2);
        yOff += 30;
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
            GFX_UTIL::DrawString(p_surf_screen, buff, xIni + xOff, yIni + yOff,
                                 txtColor, pFont);
            xOff += 70;
            GFX_UTIL::DrawString(p_surf_screen, _scoreInfo[i].Name.c_str(),
                                 xIni + xOff, yIni + yOff, txtColor, pFont);
            xOff += 300;
            snprintf(buff, sizeof(buff), "%d", _scoreInfo[i].Score);
            GFX_UTIL::DrawString(p_surf_screen, buff, xIni + xOff, yIni + yOff,
                                 txtColor, pFont);
            xOff += 100;
            snprintf(buff, sizeof(buff), "%d", _scoreInfo[i].NumCard);
            GFX_UTIL::DrawString(p_surf_screen, buff, xIni + xOff, yIni + yOff,
                                 txtColor, pFont);
            yOff += 50;
            xOff = 0;
        }

        SDL_UpdateTexture(pScreenTexture, NULL, p_surf_screen->pixels,
                          p_surf_screen->pitch);
        SDL_RenderCopy(psdlRenderer, pScreenTexture, NULL, NULL);
        SDL_RenderPresent(psdlRenderer);

        now_time = SDL_GetTicks();
        if (now_time < last_time + (1000 / 20)) {
            SDL_Delay(last_time + (1000 / 20) - now_time);
        }
    } while (!done);

    fade(p_surf_screen, p_surf_screen, 1, 1, psdlRenderer, NULL);

    return NULL;
}
