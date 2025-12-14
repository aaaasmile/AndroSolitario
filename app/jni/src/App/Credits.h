#ifndef CREDITS_H__
#define CREDITS_H__

#include <SDL3/SDL.h>

#include "ErrorInfo.h"

class FadeAction;
class GameSettings;
class MusicManager;

class CreditsView {
    enum eState { READY_TO_START, WAIT_FOR_FADING, INIT, IN_PROGRESS, DONE, TERMINATED };

   public:
    CreditsView();
    ~CreditsView();

    void Show(SDL_Surface* screen, SDL_Surface* pSurfTitle,
              SDL_Renderer* psdlRenderer);
    LPErrInApp HandleEvent(SDL_Event* pEvent);
    LPErrInApp HandleIterate(bool& done);
    bool IsOngoing() { return (_state != READY_TO_START); }
    void Reset() { _state = READY_TO_START; }

   private:
    void draw_text(const char* str);

    FadeAction* _p_FadeAction;
    bool _ignoreMouseEvent;
    GameSettings* _p_GameSettings;
    MusicManager* _p_MusicManager;
    SDL_Texture* _p_ScreenTexture;
    SDL_Renderer* _p_sdlRenderer;
    SDL_Surface* _p_surfScreen;
    SDL_Surface* _p_SurfTitle;

    eState _state;
    eState _stateAfter;
    uint32_t _scroll;
    uint32_t _line;
};
#endif
