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

}  // namespace

GameActionRecorder::GameActionRecorder()
    : _isInitialized(false), _isFinalized(false) {}

bool GameActionRecorder::Initialize(
    const std::vector<Uint8>& cardIndices, PlayersOnTable& playersOnTable,
    const std::string& settingsDirectory, const std::string& gameName,
    std::string& errorMessage) {
    if (settingsDirectory.empty() || gameName.empty()) {
        errorMessage = "Recording directory or game name is not defined";
        return false;
    }

    char timestamp[32];
    const int timestampLength =
        SDL_snprintf(timestamp, sizeof(timestamp), "%llu",
                     static_cast<unsigned long long>(SDL_GetTicksNS()));
    if (timestampLength < 0 ||
        timestampLength >= static_cast<int>(sizeof(timestamp))) {
        errorMessage = "Could not create a recording filename";
        return false;
    }

    const std::string baseFileName =
        settingsDirectory + "/" + gameName + "-match-" + timestamp;
    _filePath = baseFileName + ".json";
    if (SDL_GetPathInfo(_filePath.c_str(), NULL)) {
        _filePath = baseFileName + "-" +
                    std::to_string(SDL_GetPerformanceCounter()) + ".json";
    }
    return InitializeAtPath(cardIndices, playersOnTable, _filePath,
                            errorMessage);
}

bool GameActionRecorder::InitializeAtPath(
    const std::vector<Uint8>& cardIndices, PlayersOnTable& playersOnTable,
    const std::string& filePath, std::string& errorMessage) {
    if (filePath.empty()) {
        errorMessage = "Recording path is not defined";
        return false;
    }
    _filePath = filePath;
    _vctActions.clear();
    _isInitialized = false;
    _isFinalized = false;
    _document = invjson::Value::MakeObject();
    _document["FormatVersion"] = invjson::Value(1);

    invjson::Value deck = invjson::Value::MakeArray();
    for (size_t i = 0; i < cardIndices.size(); ++i) {
        deck.PushBack(invjson::Value(static_cast<int>(cardIndices[i])));
    }
    _document["Mazzo"] = deck;

    invjson::Value players = invjson::Value::MakeArray();
    const size_t playerCount = playersOnTable.GetNumOfPlayers();
    for (size_t i = 0; i < playerCount; ++i) {
        Player* player = playersOnTable.GetPlayerOnIndex(
            static_cast<Uint8>(i));
        invjson::Value playerData = invjson::Value::MakeObject();
        playerData["Name"] = invjson::Value(player->GetName());
        playerData["Ix"] = invjson::Value(static_cast<int>(player->GetIndex()));
        players.PushBack(playerData);
    }
    _document["Players"] = players;
    _document["Actions"] = invjson::Value::MakeArray();
    _isInitialized = true;
    _isFinalized = false;

    if (!WriteDocument(errorMessage)) {
        _isInitialized = false;
        return false;
    }
    return true;
}

void GameActionRecorder::RecordAction(
    const std::string& source, const std::string& action,
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
    _vctActions.push_back(entry);
}

bool GameActionRecorder::Finalize(const std::string& outcome,
                                  std::string& errorMessage) {
    if (!_isInitialized) {
        errorMessage = "Recorder was not initialized";
        return false;
    }
    if (_isFinalized) {
        return true;
    }

    _document["Actions"] = invjson::Value::MakeArray();
    for (size_t i = 0; i < _vctActions.size(); ++i) {
        _document["Actions"].PushBack(_vctActions[i]);
    }
    _document["Outcome"] = invjson::Value(outcome);
    if (!WriteDocument(errorMessage)) {
        return false;
    }
    _isFinalized = true;
    return true;
}

bool GameActionRecorder::WriteDocument(std::string& errorMessage) {
    if (!_isInitialized || _filePath.empty()) {
        errorMessage = "Recorder has no output file";
        return false;
    }

    const std::string json = invjson::Stringify(_document);
    SDL_IOStream* output = SDL_IOFromFile(_filePath.c_str(), "wb");
    if (output == NULL) {
        errorMessage = SDL_GetError();
        return false;
    }

    const size_t bytesWritten = SDL_WriteIO(output, json.data(), json.size());
    const bool writeSucceeded = bytesWritten == json.size();
    const std::string writeError = writeSucceeded ? "" : SDL_GetError();
    const bool closeSucceeded = SDL_CloseIO(output);
    if (!writeSucceeded) {
        errorMessage = writeError;
        return false;
    }
    if (!closeSucceeded) {
        errorMessage = SDL_GetError();
        return false;
    }
    return true;
}

const std::string& GameActionRecorder::GetFilePath() const {
    return _filePath;
}

const std::vector<invjson::Value>& GameActionRecorder::GetActions() const {
    return _vctActions;
}

void GameActionRecorder::RecordGiocataDeck(
    const std::vector<Uint8>& cardIndices) {
    invjson::Value::Object details;
    invjson::Value deck = invjson::Value::MakeArray();
    for (size_t i = 0; i < cardIndices.size(); ++i) {
        deck.PushBack(invjson::Value(static_cast<int>(cardIndices[i])));
    }
    details["Mazzo"] = deck;
    RecordAction("Engine", "ShuffledDeck", details);
}

}  // namespace invido
