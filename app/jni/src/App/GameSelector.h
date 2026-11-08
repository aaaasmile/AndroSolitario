#ifndef __GAMESELECTOR__H_
#define __GAMESELECTOR__H_

#include "GameSettings.h"
#include "Languages.h"
#include "Traits.h"

using namespace traits;

class GameSelector {
   public:
    GameSelector();
    ~GameSelector();
    GameGfxCb PrepGameGfx();
    GameHelpPagesCb PrepareGameHelpPages();
    enum Languages::eStringID GetGameTitle();
    const char* GetTitleImageFile() const;
    const char* GetSceneBackgroundFile(
        SceneBackgroundContext context) const;
    void InitializeGameSettings();
    void ApplyLoadedSettings();

   private:
    void* _p_Gfx;
};

#endif