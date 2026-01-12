#include "GameSelector.h"

#include "GameGfx/Solitario/SolitarioGfx.h"
#include "GameSettings.h"

GameSelector::GameSelector() { _p_SolitarioGfx = NULL; }

GameSelector::~GameSelector() {
    if (_p_SolitarioGfx != NULL) {
        delete _p_SolitarioGfx;
    }
}

LPErrInApp fncBind_HandleEvent(void* self, SDL_Event* pEvent,
                               const SDL_Point& targetPos) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->HandleEvent(pEvent, targetPos);
    }
    return ERR_UTIL::ErrorCreate("fncBind_HandleEvent without type");
}

LPErrInApp fncBind_HandleIterate(void* self, bool& done) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->HandleIterate(done);
    }
    return ERR_UTIL::ErrorCreate("fncBind_HandleIterate without type");
}

LPErrInApp fncBind_Initialize(void* self, SDL_Surface* pScreen,
                              UpdateScreenCb& fnUpdateScreen,
                              SDL_Window* pWindow,
                              SDL_Surface* pSceneBackground,
                              UpdateHighScoreCb& fnHighScore) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->Initialize(pScreen, fnUpdateScreen, pWindow,
                                         pSceneBackground, fnHighScore);
    }
    return ERR_UTIL::ErrorCreate("fncBind_Initialize without type");
}

LPErrInApp fncBind_Show(void* self) {
    GameSettings* pGameSettings = GameSettings::GetSettings();

    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        SolitarioGfx* pSolitarioGfx = (SolitarioGfx*)self;
        return pSolitarioGfx->Show();
    }
    return ERR_UTIL::ErrorCreate("fncBind_Show without type");
}

GameGfxCb GameSelector::PrepGameGfx() {
    GameSettings* pGameSettings = GameSettings::GetSettings();
    if (pGameSettings->GetGameTypeGfx() == GameTypeEnum::Solitario) {
        if (_p_SolitarioGfx != NULL) {
            delete _p_SolitarioGfx;
        }
        _p_SolitarioGfx = new SolitarioGfx();
        static VGameGfxCb const tc = {.HandleEvent = (&fncBind_HandleEvent),
                                      .HandleIterate = (&fncBind_HandleIterate),
                                      .Initialize = (&fncBind_Initialize),
                                      .Show = (&fncBind_Show)};
        return (GameGfxCb){.tc = &tc, .self = _p_SolitarioGfx};
    }
    
    return (GameGfxCb){.tc = NULL, .self = NULL};
}
