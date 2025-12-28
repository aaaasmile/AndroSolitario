#include "Credits.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Fading.h"
#include "GameSettings.h"
#include "MusicManager.h"

char const* credit_text[] = {
    "-SOLITARIO", /* '-' at beginning makes highlighted: */
    "",
    "PART OF THE 'CUPERATIVA' PROJECT",
    "",
    "",
    "",
    "-CONTRIBUTING ",
    "FABIO SPATARO",
    "",
    "",
    "-SPECIAL THANK",
    "IL SAN",
    "",
    "",
    "-MUSIC",
    "WATERMUSIC",
    "WOLMER AND ITS ACCORDION",
    "WINGS OF THE WIND BY GROOVYLAB80",
    "", /* The following blanks cause the screen to scroll to complete blank: */
    "",
    "",
    "",
    "-LEAD PROGRAMMER",
    "IGORRUN ON INVIDO IT",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    NULL};

char const chars[38][5][6] = {{".###.", "#..##", "#.#.#", "##..#", ".###."},

                              {"..#..", ".##..", "..#..", "..#..", ".###."},

                              {".###.", "....#", "..##.", ".#...", "#####"},

                              {".###.", "....#", "..##.", "....#", ".###."},

                              {"...#.", "..##.", ".#.#.", "#####", "...#."},

                              {"#####", "#....", "####.", "....#", "####."},

                              {".###.", "#....", "####.", "#...#", ".###."},

                              {"#####", "....#", "...#.", "..#..", ".#..."},

                              {".###.", "#...#", ".###.", "#...#", ".###."},

                              {".###.", "#...#", ".####", "....#", ".###."},

                              {".###.", "#...#", "#####", "#...#", "#...#"},

                              {"####.", "#...#", "####.", "#...#", "####."},

                              {".###.", "#....", "#....", "#....", ".###."},

                              {"####.", "#...#", "#...#", "#...#", "####."},

                              {"#####", "#....", "###..", "#....", "#####"},

                              {"#####", "#....", "###..", "#....", "#...."},

                              {".###.", "#....", "#.###", "#...#", ".###."},

                              {"#...#", "#...#", "#####", "#...#", "#...#"},

                              {".###.", "..#..", "..#..", "..#..", ".###."},

                              {"....#", "....#", "....#", "#...#", ".###."},

                              {"#..#.", "#.#..", "##...", "#.#..", "#..#."},

                              {"#....", "#....", "#....", "#....", "#####"},

                              {"#...#", "##.##", "#.#.#", "#...#", "#...#"},

                              {"#...#", "##..#", "#.#.#", "#..##", "#...#"},

                              {".###.", "#...#", "#...#", "#...#", ".###."},

                              {"####.", "#...#", "####.", "#....", "#...."},

                              {".###.", "#...#", "#.#.#", "#..#.", ".##.#"},

                              {"####.", "#...#", "####.", "#...#", "#...#"},

                              {".###.", "#....", ".###.", "....#", ".###."},

                              {"#####", "..#..", "..#..", "..#..", "..#.."},

                              {"#...#", "#...#", "#...#", "#...#", ".###."},

                              {"#...#", "#...#", ".#.#.", ".#.#.", "..#.."},

                              {"#...#", "#...#", "#.#.#", "##.##", "#...#"},

                              {"#...#", ".#.#.", "..#..", ".#.#.", "#...#"},

                              {"#...#", ".#.#.", "..#..", "..#..", "..#.."},

                              {"#####", "...#.", "..#..", ".#...", "#####"},

                              {".....", ".....", ".....", "..#..", ".#..."},

                              {"..#..", "..#..", ".....", ".....", "....."}};

CreditsView::CreditsView() {
    _scroll = 0;
    _line = 0;
    _state = CreditsView::READY_TO_START;
    _p_sdlRenderer = NULL;
    _p_surfScreen = NULL;
    _p_SurfTitle = NULL;
    _p_ScreenTexture = NULL;
    _p_GameSettings = NULL;
    _p_MusicManager = NULL;
    _p_FadeAction = new FadeAction();
}

CreditsView::~CreditsView() {
    delete _p_FadeAction;
    if (_p_ScreenTexture != NULL) {
        SDL_DestroyTexture(_p_ScreenTexture);
        _p_ScreenTexture = NULL;
    }
}

LPErrInApp CreditsView::HandleEvent(SDL_Event* pEvent) {
    if (_state != CreditsView::IN_PROGRESS) {
        return NULL;
    }
    if (pEvent->type == SDL_EVENT_KEY_DOWN) {
        if (pEvent->key.key == SDLK_ESCAPE || pEvent->key.key == SDLK_RETURN) {
            _state = CreditsView::DONE;
        }
    }
    if (pEvent->type == SDL_EVENT_FINGER_DOWN) {
        _state = CreditsView::DONE;
    }
    if (pEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (_ignoreMouseEvent) {
            return NULL;
        }
        _state = CreditsView::DONE;
    }
    return NULL;
}

LPErrInApp CreditsView::HandleIterate(bool& done) {
    SDL_Rect src, dest;
    done = false;
    if (_state == CreditsView::READY_TO_START) {
        return NULL;
    }

    if (_state == CreditsView::WAIT_FOR_FADING) {
        if (_p_FadeAction->IsInProgress()) {
            _p_FadeAction->Iterate();
            return NULL;
        }
        if (_state == _stateAfter) {
            return ERR_UTIL::ErrorCreate(
                "Next state could not be WAIT_FOR_FADING\n");
        }
        _state = _stateAfter;
    }

    if (_state == CreditsView::INIT) {
        _line = 0;
        _scroll = 0;
        if (_p_ScreenTexture != NULL) {
            SDL_DestroyTexture(_p_ScreenTexture);
        }
        _p_ScreenTexture =
            SDL_CreateTextureFromSurface(_p_sdlRenderer, _p_surfScreen);
        if (!_ignoreMouseEvent) {
            _p_FadeAction->Fade(_p_surfScreen, _p_surfScreen, 2, true,
                                _p_sdlRenderer, NULL);
            _state = CreditsView::WAIT_FOR_FADING;
            _stateAfter = CreditsView::IN_PROGRESS;
        } else {
            _p_FadeAction->InstantFade(_p_surfScreen);
            _state = CreditsView::IN_PROGRESS;
        }
        _p_MusicManager->PlayMusic(MusicManager::MUSIC_CREDITS_SND,
                                   MusicManager::LOOP_ON);

        return NULL;
    }

    if (_state == CreditsView::IN_PROGRESS) {
        Uint64 now_time = SDL_GetTicks();
        if (_lastUpTimestamp + 30 > now_time){
            //TRACE_DEBUG("Ignore iteration because too fast \n");
            return NULL;
        }
        _lastUpTimestamp = now_time;

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
        _scroll++;

        draw_text(credit_text[_line]);

        if (_scroll >= 9) {
            _scroll = 0;
            _line++;
            if (credit_text[_line] == NULL) {
                _line--;
                uint32_t elapsed_sec = (now_time / 1000) - (_start_time / 1000);
                if (elapsed_sec > 30) {
                    TRACE_DEBUG("after 30 sec, time to exit from high score\n");
                    _state = CreditsView::DONE;
                }
            }
        }
        SDL_UpdateTexture(_p_ScreenTexture, NULL, _p_surfScreen->pixels,
                          _p_surfScreen->pitch);
        SDL_RenderTexture(_p_sdlRenderer, _p_ScreenTexture, NULL, NULL);
        SDL_RenderPresent(_p_sdlRenderer);
    }

    if (_state == CreditsView::DONE) {
        TRACE("CreditsView done \n");
        _p_FadeAction->Fade(_p_surfScreen, _p_surfScreen, 1, true,
                            _p_sdlRenderer, NULL);
        _state = CreditsView::WAIT_FOR_FADING;
        _stateAfter = CreditsView::TERMINATED;
    }

    if (_state == CreditsView::TERMINATED) {
        if (_p_MusicManager->IsPlayingMusic()) {
            _p_MusicManager->StopMusic(300);
        }
        TRACE("CreditsView terminated \n");
        done = true;
        _state = CreditsView::READY_TO_START;
    }

    return NULL;
}

void CreditsView::Show(SDL_Surface* p_surf_screen, SDL_Surface* pSurfTitle,
                       SDL_Renderer* psdlRenderer) {
    _p_sdlRenderer = psdlRenderer;
    _p_surfScreen = p_surf_screen;
    _p_SurfTitle = pSurfTitle;

    _p_GameSettings = GameSettings::GetSettings();
    _p_MusicManager = _p_GameSettings->GetMusicManager();
    _ignoreMouseEvent =
        _p_GameSettings->InputType == InputTypeEnum::TouchWithoutMouse;
    _state = CreditsView::INIT;
    _start_time = SDL_GetTicks();
}

void CreditsView::draw_text(char const* str) {
    int i, c, x, y, cur_x, start, hilite;
    SDL_Rect dest;
    Uint8 r, g, b;

    if (str[0] == '-') {
        start = 1;
        hilite = 1;
    } else {
        start = 0;
        hilite = 0;
    }

    cur_x = (_p_surfScreen->w - ((strlen(str) - start) * 18)) / 2;

    for (i = start; i < (int)strlen(str); i++) {
        c = -1;

        if (str[i] >= '0' && str[i] <= '9')
            c = str[i] - '0';
        else if (str[i] >= 'A' && str[i] <= 'Z')
            c = str[i] - 'A' + 10;
        else if (str[i] == ',')
            c = 36;
        else if (str[i] == '\'')
            c = 37;

        if (c != -1) {
            for (y = 0; y < 5; y++) {
                if (hilite == 0) {
                    r = 255 - ((_line * y) % 256);
                    g = 255 / (y + 2);
                    b = (_line * _line * 2) % 256;
                } else {
                    r = 128;
                    g = 192;
                    b = 255 - (y * 40);
                }

                for (x = 0; x < 5; x++) {
                    if (chars[c][y][x] == '#') {
                        dest.x = cur_x + (x * 3);
                        dest.y = ((_p_surfScreen->h - (5 * 3)) + (y * 3) +
                                  (18 - _scroll * 2));
                        dest.w = 3;
                        dest.h = 3;

                        SDL_FillSurfaceRect(
                            _p_surfScreen, &dest,
                            SDL_MapRGB(SDL_GetPixelFormatDetails(
                                           _p_surfScreen->format),
                                       NULL, r, g, b));
                    }
                }
            }
        }
        cur_x = cur_x + 18;
    }
}
