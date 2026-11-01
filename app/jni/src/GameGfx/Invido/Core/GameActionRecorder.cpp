#include "GameActionRecorder.h"

#include <SDL3/SDL.h>

#include "Player.h"
#include "PlayersOnTable.h"

namespace invido {
namespace {

void AddObjectField(invjson::Value& object, const std::string& name,
                    const invjson::Value& value) {
    object[name] = value;
}

}  // namespace private

GameActionRecorder::GameActionRecorder()
    : _isInitialized(false), _isFinalized(false) {}

LPErrInApp GameActionRecorder::Initialize(PlayersOnTable& playersOnTable,
                                          const std::string& settingsDirectory,
                                          const std::string& gameName) {
    if (settingsDirectory.empty() || gameName.empty()) {
        return ERR_UTIL::ErrorCreate(
            "Recording directory or game name is not defined");
    }
    if (!SDL_CreateDirectory(settingsDirectory.c_str())) {
        return ERR_UTIL::ErrorCreate(
            "Unable to create recording directory '%s': %s",
            settingsDirectory.c_str(), SDL_GetError());
    }

    char timestamp[32];
    const int timestampLength =
        SDL_snprintf(timestamp, sizeof(timestamp), "%llu",
                     static_cast<unsigned long long>(SDL_GetTicksNS()));
    if (timestampLength < 0 ||
        timestampLength >= static_cast<int>(sizeof(timestamp))) {
        return ERR_UTIL::ErrorCreate("Could not create a recording filename");
    }

    const std::string baseFileName =
        settingsDirectory + "/" + gameName + "-match-" + timestamp;
    _filePath = baseFileName + ".json";
    if (SDL_GetPathInfo(_filePath.c_str(), NULL)) {
        _filePath = baseFileName + "-" +
                    std::to_string(SDL_GetPerformanceCounter()) + ".json";
    }
    return InitializeAtPath(playersOnTable, _filePath);
}

LPErrInApp GameActionRecorder::InitializeAtPath(PlayersOnTable& playersOnTable,
                                                const std::string& filePath) {
    if (filePath.empty()) {
        return ERR_UTIL::ErrorCreate("Recording path is not defined");
    }
    _filePath = filePath;
    _vctActions.clear();
    _isInitialized = false;
    _isFinalized = false;
    _document = invjson::Value::MakeObject();
    _document["FormatVersion"] = invjson::Value(1);

    invjson::Value players = invjson::Value::MakeArray();
    const size_t playerCount = playersOnTable.GetNumOfPlayers();
    for (size_t i = 0; i < playerCount; ++i) {
        Player* player = playersOnTable.GetPlayerOnIndex(static_cast<Uint8>(i));
        invjson::Value playerData = invjson::Value::MakeObject();
        playerData["Name"] = invjson::Value(player->GetName());
        playerData["Ix"] = invjson::Value(static_cast<int>(player->GetIndex()));
        players.PushBack(playerData);
    }
    _document["Players"] = players;
    _document["Actions"] = invjson::Value::MakeArray();
    _isInitialized = true;
    _isFinalized = false;

    LPErrInApp error = FlashActions();
    if (error != NULL) {
        _isInitialized = false;
        return error;
    }
    return NULL;
}

void GameActionRecorder::RecordAction(const std::string& source,
                                      const std::string& action,
                                      const invjson::Value::Object& details) {
    if (!_isInitialized || _isFinalized) {
        return;
    }

    invjson::Value entry = invjson::Value::MakeObject();
    entry["Sequence"] = invjson::Value(static_cast<int>(_vctActions.size()));
    entry["Source"] = invjson::Value(source);
    entry["Action"] = invjson::Value(action);
    for (invjson::Value::Object::const_iterator it = details.begin();
         it != details.end(); ++it) {
        AddObjectField(entry, it->first, it->second);
    }
    _document["Actions"].PushBack(entry);
    _vctActions.push_back(entry);
}

LPErrInApp GameActionRecorder::FlashActions() {
    if (!_isInitialized) {
        return ERR_UTIL::ErrorCreate("Recorder was not initialized");
    }
    return WriteDocument();
}

LPErrInApp GameActionRecorder::Finalize(const std::string& outcome) {
    if (!_isInitialized) {
        return ERR_UTIL::ErrorCreate("Recorder was not initialized");
    }
    if (_isFinalized) {
        return NULL;
    }

    _document["Outcome"] = invjson::Value(outcome);
    LPErrInApp error = FlashActions();
    if (error != NULL) {
        return error;
    }
    _isFinalized = true;
    return NULL;
}

bool GameActionRecorder::IsInitialized() const { return _isInitialized; }

LPErrInApp GameActionRecorder::WriteDocument() {
    if (!_isInitialized || _filePath.empty()) {
        return ERR_UTIL::ErrorCreate("Recorder has no output file");
    }

    const std::string json = invjson::Stringify(_document);
    SDL_IOStream* output = SDL_IOFromFile(_filePath.c_str(), "wb");
    if (output == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to open recorder file %s",
                                     SDL_GetError());
    }

    const size_t bytesWritten = SDL_WriteIO(output, json.data(), json.size());
    if (bytesWritten != json.size()) {
        const std::string writeError = SDL_GetError();
        SDL_CloseIO(output);
        return ERR_UTIL::ErrorCreate("Unable to write recorder JSON: %s",
                                     writeError.c_str());
    }
    if (!SDL_CloseIO(output)) {
        return ERR_UTIL::ErrorCreate("Unable to close recorder file: %s",
                                     SDL_GetError());
    }
    return NULL;
}

const std::string& GameActionRecorder::GetFilePath() const { return _filePath; }

const std::vector<invjson::Value>& GameActionRecorder::GetActions() const {
    return _vctActions;
}

LPErrInApp GameActionRecorder::RecordGiocataDeck(
    const std::vector<Uint8>& cardIndices) {
    invjson::Value::Object details;
    invjson::Value deck = invjson::Value::MakeArray();
    for (size_t i = 0; i < cardIndices.size(); ++i) {
        deck.PushBack(invjson::Value(static_cast<int>(cardIndices[i])));
    }
    details["Mazzo"] = deck;
    RecordAction("Engine", "ShuffledDeck", details);
    if (!_isInitialized || _isFinalized) {
        return NULL;
    }
    return FlashActions();
}

}  // namespace invido
