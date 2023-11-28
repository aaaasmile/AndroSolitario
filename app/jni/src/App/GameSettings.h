#ifndef __GAMESETTINGS__H_
#define __GAMESETTINGS__H_

#include <string>
#include "SDL.h"
#include "DeckType.h"
#include "ErrorInfo.h"
#include "Languages.h"

enum BackgroundTypeEnum { Commessaggio = 0, Mantova = 1, Black = 2 };

class GameSettings {
   public:
    std::string PlayerName;
    int Level;
    DeckType DeckTypeVal;
    Languages::eLangId CurrentLanguage;
    bool MusicEnabled;
    std::string SettingsDir;
    std::string GameName;
    BackgroundTypeEnum BackgroundType;

   public:
    GameSettings() {
        PlayerName = "";
        Level = 1;
        DeckTypeVal.SetType(eDeckType::PIACENTINA);
        CurrentLanguage = Languages::LANG_ITA;
        MusicEnabled = true;
        SettingsDir = "";
        BackgroundType = BackgroundTypeEnum::Commessaggio;
    }
    LPErrInApp LoadSettings();
    LPErrInApp SaveSettings();
    void CalcDisplaySize();
    void GetTouchPoint(SDL_TouchFingerEvent& tfinger, SDL_Point* pPoint);
    int GetScreenHeight() { return _screenRect.h; }
    int GetScreenWidth() { return _screenRect.w; }

   private:
    void setSettingFileName();
    SDL_Rect _screenRect = {0};
};

typedef GameSettings* LPGameSettings;

namespace GAMESET {
LPGameSettings GetSettings();
const char* GetHomeFolder();
LPErrInApp CreateHomeFolderIfNotExists(bool& dirCreated);
const char* GetExeAppFolder();
void GetNameWithAssets(const char* src_path, std::string& res);
}
#endif
