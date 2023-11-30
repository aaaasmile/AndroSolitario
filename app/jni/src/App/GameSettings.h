#ifndef __GAMESETTINGS__H_
#define __GAMESETTINGS__H_

#include <string>

#include "DeckType.h"
#include "ErrorInfo.h"
#include "Languages.h"
#include "SDL.h"

enum BackgroundTypeEnum { Commessaggio = 0, Mantova = 1, Black = 2 };
enum InputTypeEnum { Mouse = 0, TouchWithoutMouse = 1 };

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
    InputTypeEnum InputType;

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
    bool NeedScreenMagnify() {
        return (_screenRect.h > 1200 || _screenRect.w > 1200);
    }
    int GetSizeFontSmall() { return _fontSmallSize; }
    int GetSizeFontBig() { return _fontBigSize; }
    void SetFontSize(int s, int b) {
        _fontBigSize = b;
        _fontSmallSize = s;
    }
    void UseBigFontSize() {
        _fontSmallSize = 22;
        _fontBigSize = 46;
    }

   private:
    void setSettingFileName();
    SDL_Rect _screenRect = {0};
    int _fontSmallSize = 11;
    int _fontBigSize = 22;
};

typedef GameSettings* LPGameSettings;

namespace GAMESET {
LPGameSettings GetSettings();
const char* GetHomeFolder();
LPErrInApp CreateHomeFolderIfNotExists(bool& dirCreated);
const char* GetExeAppFolder();
void GetNameWithAssets(const char* src_path, std::string& res);
}  // namespace GAMESET
#endif
