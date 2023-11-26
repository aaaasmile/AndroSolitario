#ifndef __GAMESETTINGS__H_
#define __GAMESETTINGS__H_

#include <string>

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
        PlayerName = "Anonimo";
        Level = 1;
        DeckTypeVal.SetType(eDeckType::PIACENTINA);
        CurrentLanguage = Languages::LANG_ITA;
        MusicEnabled = true;
        SettingsDir = "";
        BackgroundType = BackgroundTypeEnum::Commessaggio;
    }
    LPErrInApp LoadSettings();
    LPErrInApp SaveSettings();
private:
    void setSettingFileName();
};

typedef GameSettings* LPGameSettings;

namespace GAMESET {
    LPGameSettings GetSettings();
    const char* GetHomeFolder();
    LPErrInApp CreateHomeFolderIfNotExists(bool& dirCreated);
    const char* GetExeAppFolder();
    const char* GetNameWithAssets(const char *src_path);
}
#endif
