#ifndef _MUSICMANAGER__H_
#define _MUSICMANAGER__H_

#include "ErrorInfo.h"
#include "SDL_mixer.h"
#include "WinTypeGlobal.h"

class MusicManager {
   public:
    enum { MUSIC_INIT_SND, MUSIC_CREDITS_SND, MUSIC_PLAY_SND, NUM_OF_SOUNDS };
    enum eLoopType { LOOP_ON, LOOP_OFF };
    enum { NOTHING, NUM_OF_WAV };
    MusicManager();
    virtual ~MusicManager();

    void Initialize(bool musicEnabled);
    void StopMusic(int fadingMs);
    bool PlayMusic(int iID, eLoopType eVal);
    void PlayCurrentMusic();
    bool PlayEffect(int iID);
    LPErrInApp LoadMusicRes();
    bool IsPlayingMusic();
    void SetVolumeMusic(int iVal);
    void EnableMusic() {
        if (_musicHardwareAvail) {
            _musicDisabled = false;
        }
    }
    void DisableMusic() { _musicDisabled = true; }
    bool IsMusicEnabled() { return !_musicDisabled; }

   private:
    Mix_Chunk* _p_MusicsWav[NUM_OF_WAV];
    Mix_Music* _p_Musics[NUM_OF_SOUNDS];
    bool _musicDisabled;
    bool _musicHardwareAvail;
    int _currentMusicID;
    eLoopType _currentLoop;
};

#endif
