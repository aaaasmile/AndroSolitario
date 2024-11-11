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
    "-LEAD PROGRAMMER",
    "THE AUTHOR OF INVIDO,IT",
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
    "",
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

static void draw_text(const char* str, int scroll, SDL_Surface* screen);

static int g_line = 0;

void credits(SDL_Surface* p_surf_screen, SDL_Surface* pSurfTitle,
             SDL_Renderer* psdlRenderer, MusicManager* pMusicManager) {
    bool done;
    uint32_t scroll;
    SDL_Rect src, dest;
    SDL_Event event;
    Uint32 last_time, now_time;
    SDL_Keycode key;
    LPGameSettings pGameSettings = GAMESET::GetSettings();

    SDL_Texture* pScreenTexture =
        SDL_CreateTextureFromSurface(psdlRenderer, p_surf_screen);

    if (pGameSettings->InputType != InputTypeEnum::TouchWithoutMouse) {
        Fade(p_surf_screen, p_surf_screen, 2, 1, psdlRenderer, NULL);
    } else {
        InstantFade(p_surf_screen);
    }
    pMusicManager->PlayMusic(MusicManager::MUSIC_CREDITS_SND,
                             MusicManager::LOOP_ON);

    dest.x = (p_surf_screen->w - pSurfTitle->w) / 2;
    dest.y = 0;
    dest.w = pSurfTitle->w;
    dest.h = pSurfTitle->h;

    SDL_BlitSurface(pSurfTitle, NULL, p_surf_screen, &dest);
    done = 0;
    scroll = 0;
    g_line = 0;
    do {
        last_time = SDL_GetTicks();
        while (SDL_PollEvent(&event) > 0) {
            if (event.type == SDL_KEYDOWN) {
                key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    done = 1;
                }
            }
            if (event.type == SDL_FINGERDOWN) {
                done = true;
            }
        }

        src.x = 0;
        src.y = (pSurfTitle->h) + 2;
        src.w = p_surf_screen->w;
        src.h = p_surf_screen->h - (pSurfTitle->h);

        dest.x = 0;
        dest.y = (pSurfTitle->h);
        dest.w = src.w;
        dest.h = src.h;

        SDL_BlitSurface(p_surf_screen, &src, p_surf_screen, &dest);

        dest.x = 0;
        dest.y = (p_surf_screen->h) - 2;
        dest.w = p_surf_screen->w;
        dest.h = 2;

        SDL_FillRect(p_surf_screen, &dest,
                     SDL_MapRGB(p_surf_screen->format, 0, 0, 0));

        scroll++;

        draw_text(credit_text[g_line], scroll, p_surf_screen);

        if (scroll >= 9) {
            scroll = 0;
            g_line++;

            if (credit_text[g_line] == NULL)
                done = 1;
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

    Fade(p_surf_screen, p_surf_screen, 1, 1, psdlRenderer, NULL);

    if (pMusicManager->IsPlayingMusic()) {
        pMusicManager->StopMusic(300);
    }
}

static void draw_text(char const* str, int scroll, SDL_Surface* screen) {
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

    cur_x = (screen->w - ((strlen(str) - start) * 18)) / 2;

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
                    r = 255 - ((g_line * y) % 256);
                    g = 255 / (y + 2);
                    b = (g_line * g_line * 2) % 256;
                } else {
                    r = 128;
                    g = 192;
                    b = 255 - (y * 40);
                }

                for (x = 0; x < 5; x++) {
                    if (chars[c][y][x] == '#') {
                        dest.x = cur_x + (x * 3);
                        dest.y = ((screen->h - (5 * 3)) + (y * 3) +
                                  (18 - scroll * 2));
                        dest.w = 3;
                        dest.h = 3;

                        SDL_FillRect(screen, &dest,
                                     SDL_MapRGB(screen->format, r, g, b));
                    }
                }
            }
        }
        cur_x = cur_x + 18;
    }
}
