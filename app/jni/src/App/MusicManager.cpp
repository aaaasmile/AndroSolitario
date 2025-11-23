#include "MusicManager.h"

#include <SDL3/SDL.h>

#include "GameSettings.h"

static const char* lpszaSound_filenames[MusicManager::NUM_OF_SOUNDS] = {
    DATA_PREFIX "music/wolmer-invido.ogg", DATA_PREFIX "music/watermusic.ogg",
    DATA_PREFIX "music/wings-of-the-wind.ogg"};

static const char* lpszaEffects_filenames[MusicManager::NUM_OF_WAV] = {
    DATA_PREFIX "music/click_over.wav"};

MusicManager::MusicManager() {
    for (int i = 0; i < NUM_OF_SOUNDS; i++) {
        _p_Musics[i] = 0;
    }
    for (int j = 0; j < NUM_OF_WAV; j++) {
        _p_MusicsWav[j] = 0;
    }
    _currentMusicID = 0;
    _currentLoop = LOOP_ON;
    _musicDisabled = true;
    _musicPaused = false;
}

MusicManager::~MusicManager() { TRACE_DEBUG("Destroy music manager\n"); }

void MusicManager::Terminate() {
    StopMusic(0);
    for (int i = 0; i < NUM_OF_SOUNDS; i++) {
        Mix_FreeMusic(_p_Musics[i]);
    }
    for (int j = 0; j < NUM_OF_WAV; j++) {
        Mix_FreeChunk(_p_MusicsWav[j]);
    }
    Mix_CloseAudio();
    Mix_Quit();

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    TRACE_DEBUG("Audio sub system shutdown\n");
}

LPErrInApp MusicManager::Initialize(bool musicEnabled) {
    _musicDisabled = true;
    _musicHardwareAvail = false;
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        return ERR_UTIL::ErrorCreate("Error on audio initialize: %s",
                                     SDL_GetError());
    }
    SDL_AudioSpec audio_spec;
    audio_spec.freq = 44100;  // Sampling frequency in Hz
    audio_spec.format =
        SDL_AUDIO_S16LE;      // Sample format (16-bit signed integer)
    audio_spec.channels = 2;  // Number of channels (Stereo)

    if (!Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec)) {
        return ERR_UTIL::ErrorCreate("Error on Mix_OpenAudio: %s",
                                     SDL_GetError());

    } else {
        _musicHardwareAvail = true;
        _musicDisabled = !musicEnabled;
    }

    if (_musicDisabled) {
        TRACE_DEBUG(
            "[WARN] Music is disabled (by settings? %s, hardware audio? %s)\n",
            !musicEnabled ? "true" : "false",
            _musicHardwareAvail ? "Ok" : "Failed");
    } else {
        TRACE("Music system initialized OK\n");
    }
    return NULL;
}

LPErrInApp MusicManager::LoadMusicRes() {
    if (!_musicHardwareAvail){
        TRACE_DEBUG("Avoid load music because Audio Hardware \n");
        return NULL;
    }
    for (int i = 0; i < NUM_OF_SOUNDS; i++) {
        STRING strFileTmp2 = lpszaSound_filenames[i];
#ifdef WIN32
        STRING exeDirPath = GAMESET::GetExeAppFolder();
        STRING strFileFullPath = exeDirPath + '/' + strFileTmp2;
#else
        STRING strFileFullPath = strFileTmp2;
#endif
        TRACE_DEBUG("Loading music part %s\n", strFileFullPath.c_str());
        _p_Musics[i] = Mix_LoadMUS(strFileFullPath.c_str());
        if (_p_Musics[i] == NULL) {
            return ERR_UTIL::ErrorCreate(
                "Unable to load %s music resource, error: %s\n",
                strFileFullPath.c_str(), SDL_GetError());
        }
    }

    for (int j = 0; j < NUM_OF_WAV; j++) {
        if (lpszaEffects_filenames[j] != NULL) {
            _p_MusicsWav[j] = Mix_LoadWAV(lpszaEffects_filenames[j]);
            if (_p_MusicsWav[j] == NULL) {
                return ERR_UTIL::ErrorCreate(
                    "Unable to load %s wav resource, error: %s\n",
                    lpszaEffects_filenames[j], SDL_GetError());
            }
        }
    }
    return NULL;
}

void MusicManager::StopMusic(int fadingMs) {
    if (_musicDisabled) {
        return;
    }
    Mix_FadeOutMusic(fadingMs);
    Mix_HaltMusic();
}

void MusicManager::PauseMusic() {
    TRACE_DEBUG("Pause music \n");
    if (_musicDisabled) {
        return;
    }
    Mix_PauseMusic();
    _musicPaused = true;
}

void MusicManager::ResumeMusic() {
    TRACE_DEBUG("Resume music \n");
    if (_musicDisabled) {
        return;
    }
    Mix_ResumeMusic();
    _musicPaused = false;
}

bool MusicManager::IsPlayingMusic() {
    if (_musicDisabled) {
        return true;
    }
    return Mix_PlayingMusic();
}

bool MusicManager::PlayMusic(int iID, eLoopType eVal) {
    if (_musicDisabled) {
        TRACE_DEBUG("Ignore PlayMusic because is disabled\n");
    } else {
        TRACE_DEBUG("Playing music id: %d, loop: %d\n", iID, eVal);
    }

    if (iID < 0 || iID >= NUM_OF_SOUNDS) {
        return false;
    }
    if (_p_Musics[iID] == 0) {
        return false;
    }
    _currentMusicID = iID;
    _currentLoop = eVal;
    if (_musicDisabled) {
        return false;
    }

    if (eVal == LOOP_OFF) {
        Mix_PlayMusic(_p_Musics[iID], 0);
    } else {
        Mix_PlayMusic(_p_Musics[iID], -1);
    }

    return true;
}

void MusicManager::PlayCurrentMusic() {
    if (_musicDisabled) {
        return;
    }
    PlayMusic(_currentMusicID, _currentLoop);
}

bool MusicManager::PlayEffect(int iID) {
    if (_musicDisabled) {
        return false;
    }
    if (iID < 0 || iID >= NUM_OF_WAV) {
        return false;
    }
    Mix_PlayChannel(-1, _p_MusicsWav[iID], 0);
    return true;
}

void MusicManager::SetVolumeMusic(int iVal) {
    if (_musicDisabled) {
        return;
    }
    Mix_VolumeMusic(iVal);
}
