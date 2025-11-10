#ifndef _HIGHSCORE_H__
#define _HIGHSCORE_H__

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

#include "ErrorInfo.h"
#include "Languages.h"

typedef struct {
    std::string Name;
    uint16_t Score;
    uint8_t NumCard;
} ScoreInfo;

class HighScore {
    enum { NUMOFSCORE = 10 };

   public:
    HighScore();
    ~HighScore();
    LPErrInApp Load();
    LPErrInApp Save();
    LPErrInApp SaveScore(int score, int numCard);
    LPErrInApp Show(SDL_Surface* screen, SDL_Surface* pSurfTitle,
                    SDL_Renderer* psdlRenderer);

   private:
    ScoreInfo _scoreInfo[NUMOFSCORE];
};

#endif
