#ifndef __GAMESETTINGS__H_
#define __GAMESETTINGS__H_

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

#include "DeckType.h"
#include "ErrorInfo.h"
#include "Languages.h"

enum BackgroundTypeEnum { Commessaggio = 0, Mantova = 1, Black = 2 };
enum InputTypeEnum { Mouse = 0, TouchWithoutMouse = 1 };
enum GameTypeEnum { Solitario };

class MusicManager;

class GameSettings {
   public:
    std::string PlayerName;
    DeckType DeckTypeVal;
    Languages::eLangId CurrentLanguage;
    bool MusicEnabled;
    std::string SettingsDir;
    std::string GameName;
    BackgroundTypeEnum BackgroundType;
    InputTypeEnum InputType;

   public:
    static GameSettings* GetSettings();
    ~GameSettings();

    LPErrInApp LoadSettings();
    LPErrInApp SaveSettings();
    LPErrInApp LoadFonts();
    void SetDisplaySize(int w, int h);
    void GetTouchPoint(SDL_TouchFingerEvent& tfinger, SDL_Point* pPoint);
    int GetScreenHeight() { return _screenRect.h; }
    int GetScreenWidth() { return _screenRect.w; }
    int GetSizeFontSmall() { return _fontSmallSize; }
    int GetSizeFontBig() { return _fontBigSize; }
    int GetSizeFontSym() { return _fontSymSize; }
    void SetFontSize(int s, int b) {
        _fontBigSize = b;
        _fontSmallSize = s;
    }
    void UseBigFontSize() {
        _fontSmallSize = 22;
        _fontBigSize = 46;
        _fontSymSize = 36;
    }
    TTF_Font* GetFontDjvBig() { return _p_fontDejBig; }
    TTF_Font* GetFontDjvBoldBig() { return _p_fontAblkBig; }
    TTF_Font* GetFontDjvMedium() { return _p_fontDejMedium; }
    TTF_Font* GetFontDjvSmall() { return _p_fontDejSmall; }
    TTF_Font* GetFontDjvUnderscoreSmall() { return _p_fontDejUnderscoreSmall; }
    TTF_Font* GetFontSymb() { return _p_fontSymb; }

    Languages* GetLanguageMan();
    void SetCurrentLang();

    MusicManager* GetMusicManager() { return _p_MusicManager; }
    LPErrInApp InitMusicManager();
    void TerminateMusicManager();

    void SetPortraitDevMode(bool val) {
        _portraitDevMode = val;
        _portraitMode = val;
    }
    void SetPortraitNarrowMode(bool val) {
        _portraitNarrowMode = val;
        _portraitMode = val;
    }
    void SetPortraitWideMode(bool val) {
        _portraitWideMode = val;
        _portraitMode = val;
    }
    bool IsPortrait() { return _portraitMode; }
    bool IsNarrowPortrait() { return _portraitNarrowMode; }
    bool IsWidePortrait() { return _portraitWideMode; }
    bool IsDEVPortrait() { return _portraitDevMode; }

    GameTypeEnum GetGameTypeGfx() { return _gameType; };

   private:
    GameSettings();
    LPErrInApp setSettingFileName();

   private:
    SDL_Rect _screenRect = {0, 0, 0, 0};
    int _fontSmallSize = 11;
    int _fontMediumSize = 18;
    int _fontBigSize = 22;
    int _fontSymSize = 24;
    TTF_Font* _p_fontDejSmall;
    TTF_Font* _p_fontDejUnderscoreSmall;
    TTF_Font* _p_fontMedium;
    TTF_Font* _p_fontDejBig;
    TTF_Font* _p_fontAblkBig;
    TTF_Font* _p_fontSymb;
    TTF_Font* _p_fontDejMedium;
    Languages* _p_Languages;
    MusicManager* _p_MusicManager;
    bool _portraitMode = false;
    bool _portraitNarrowMode = false;
    bool _portraitWideMode = false;
    bool _portraitDevMode = false;
    GameTypeEnum _gameType = GameTypeEnum::Solitario;
};

typedef GameSettings* LPGameSettings;

namespace GAMESET {
const char* GetHomeFolder();
LPErrInApp CreateHomeFolderIfNotExists(bool& dirCreated);
const char* GetExeAppFolder();
void GetNameWithAssets(const char* src_path, std::string& res);
}  // namespace GAMESET
#endif
