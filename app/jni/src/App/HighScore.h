#ifndef _HIGHSCORE_H__
#define _HIGHSCORE_H__

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

#include "ErrorInfo.h"
#include "Languages.h"

class FadeAction;
class MusicManager;
class GameSettings;

typedef struct {
    std::string Name;
    uint16_t Score;
    uint8_t NumCard;
} ScoreInfo;

class HighScore {
    enum { NUMOFSCORE = 10 };
    enum eState {
        READY_TO_START,
        WAIT_FOR_FADING,
        INIT,
        IN_PROGRESS,
        IN_PROGRESS_WAIT,
        DONE,
        TERMINATED
    };

   public:
    HighScore();
    ~HighScore();
    LPErrInApp Load();
    LPErrInApp Save();
    LPErrInApp SaveScore(int64_t score, int numCard);
    LPErrInApp Show(SDL_Surface* screen, SDL_Surface* pSurfTitle,
                    SDL_Renderer* psdlRenderer);
    LPErrInApp HandleEvent(SDL_Event* pEvent);
    LPErrInApp HandleIterate(bool& done);
    bool IsOngoing() { return (_state != READY_TO_START); }
    void Reset() { _state = READY_TO_START; }

   private:
    ScoreInfo _scoreInfo[NUMOFSCORE];
    FadeAction* _p_FadeAction;
    SDL_Renderer* _p_sdlRenderer;
    SDL_Surface* _p_surfScreen;
    SDL_Surface* _p_SurfTitle;
    SDL_Texture* _p_ScreenTexture;
    eState _state;
    GameSettings* _p_GameSettings;
    MusicManager* _p_MusicManager;
    bool _ignoreMouseEvent;
    Uint64 _start_time;
    eState _stateAfter;
    Uint64 _lastUpTimestamp;
};

#endif
