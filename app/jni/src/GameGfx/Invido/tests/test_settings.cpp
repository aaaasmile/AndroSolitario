#include <SDL3/SDL.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>

#include "GameSettings.h"

namespace {

const char* const kSettingsFileName = "InvidoSettingsPersistenceTest.bin";
const char* const kGameName = "InvidoSettingsPersistenceTest";

void writeSettingsField(SDL_IOStream* stream, const void* data, size_t size) {
    if (SDL_WriteIO(stream, data, size) != size) {
        throw std::runtime_error("Unable to write test settings file");
    }
}

void writeSettingsFileWithoutExtensionBackground() {
    SDL_IOStream* stream = SDL_IOFromFile(kSettingsFileName, "wb");
    if (stream == NULL) {
        throw std::runtime_error("Unable to create test settings file");
    }

    const uint8_t deckIndex = 0;
    const uint8_t language = Languages::LANG_ITA;
    const uint8_t musicEnabled = 1;
    const uint8_t legacyBackgroundType = BackgroundTypeEnum::Image;
    const std::string playerName = "Tester";
    const uint8_t playerNameLength =
        static_cast<uint8_t>(playerName.size());
    const char extension[] = "GOPT";
    const uint8_t extensionVersion = 1;
    const uint16_t optionCount = 1;
    const std::string optionKey = GAMESET::SceneBackgroundOptionKey;
    const std::string optionValue = "Commessaggio";
    const uint8_t optionKeyLength = static_cast<uint8_t>(optionKey.size());
    const uint16_t optionValueLength =
        static_cast<uint16_t>(optionValue.size());

    try {
        writeSettingsField(stream, &deckIndex, sizeof(deckIndex));
        writeSettingsField(stream, &language, sizeof(language));
        writeSettingsField(stream, &musicEnabled, sizeof(musicEnabled));
        writeSettingsField(stream, &legacyBackgroundType,
                           sizeof(legacyBackgroundType));
        writeSettingsField(stream, &playerNameLength,
                           sizeof(playerNameLength));
        writeSettingsField(stream, playerName.data(), playerNameLength);
        writeSettingsField(stream, extension, sizeof(extension) - 1);
        writeSettingsField(stream, &extensionVersion, sizeof(extensionVersion));
        writeSettingsField(stream, &optionCount, sizeof(optionCount));
        writeSettingsField(stream, &optionKeyLength, sizeof(optionKeyLength));
        writeSettingsField(stream, &optionValueLength,
                           sizeof(optionValueLength));
        writeSettingsField(stream, optionKey.data(), optionKeyLength);
        writeSettingsField(stream, optionValue.data(), optionValueLength);
    } catch (...) {
        SDL_CloseIO(stream);
        throw;
    }

    if (!SDL_CloseIO(stream)) {
        throw std::runtime_error("Unable to close test settings file");
    }
}

}  // namespace

void testSettingsPersistence() {
    GameSettings* settings = GameSettings::GetSettings();
    settings->SettingsDir = ".";
    settings->SetGameTypeGfx(GameTypeEnum::Invido, kGameName);
    settings->GameOptions.clear();
    settings->CurrentLanguage = Languages::LANG_ENG;

    try {
        writeSettingsFileWithoutExtensionBackground();

        LPErrInApp error = settings->LoadSettings();
        if (error != NULL) {
            throw std::runtime_error(error->ErrorText);
        }
        assert(settings->CurrentLanguage == Languages::LANG_ITA);

        settings->CurrentLanguage = Languages::LANG_DIAL_BREDA;
        error = settings->SaveSettings();
        if (error != NULL) {
            throw std::runtime_error(error->ErrorText);
        }

        settings->CurrentLanguage = Languages::LANG_ENG;
        error = settings->LoadSettings();
        if (error != NULL) {
            throw std::runtime_error(error->ErrorText);
        }
        assert(settings->CurrentLanguage == Languages::LANG_DIAL_BREDA);
        assert(settings->GameOptions[GAMESET::SceneBackgroundOptionKey] ==
               "Commessaggio");
    } catch (...) {
        std::remove(kSettingsFileName);
        throw;
    }

    std::remove(kSettingsFileName);
}
