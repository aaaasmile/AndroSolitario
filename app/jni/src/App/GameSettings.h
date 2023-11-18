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
    BackgroundTypeEnum BackgroundType;

public:
    GameSettings() {
        PlayerName = "Anonimo";
        Level = 1;
        DeckTypeVal.SetType(eDeckType::PIACENTINA);
        CurrentLanguage = Languages::LANG_ITA;
        MusicEnabled = false;
        SettingsDir = "";
        BackgroundType = BackgroundTypeEnum::Commessaggio;
    }
};

typedef GameSettings* LPGameSettings;

namespace GAMESET {
    LPGameSettings GetSettings();
    const char* GetHomeSolitarioFolder();
    LPErrInApp CreateHomeSolitarioFolderIfNotExists(bool& dirCreated);
    const char* GetExeSolitarioFolder();
}
#endif
