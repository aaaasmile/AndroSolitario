#include "GameActionReplayer.h"

#include <limits>

#include <SDL3/SDL.h>

namespace invido {
namespace {

const int kSupportedFormatVersion = 1;

LPErrInApp InvalidRecording(const std::string& reason) {
    return ERR_UTIL::ErrorCreate("Invalid GameActionRecorder JSON: %s",
                                 reason.c_str());
}

bool ReadInteger(const invjson::Value* value, int& result) {
    if (value == NULL || value->GetType() != invjson::Value::Type::Number) {
        return false;
    }

    const double number = value->AsNumber();
    if (number < 0 || number > std::numeric_limits<int>::max() ||
        number != static_cast<int>(number)) {
        return false;
    }
    result = static_cast<int>(number);
    return true;
}

bool ReadString(const invjson::Value* value, std::string& result) {
    if (value == NULL || value->GetType() != invjson::Value::Type::String) {
        return false;
    }
    result = value->AsString();
    return true;
}

}  // namespace

GameActionReplayer::GameActionReplayer()
    : _isLoaded(false),
      _hasMatchStartPlayer(false),
      _matchStartPlayerIndex(NOT_VALID_INDEX),
      _nextGiocataIndex(0),
      _nextActionIndex(0),
      _hasPendingAction(false) {}

void GameActionReplayer::Reset() {
    _isLoaded = false;
    _hasMatchStartPlayer = false;
    _matchStartPlayerIndex = NOT_VALID_INDEX;
    _nextGiocataIndex = 0;
    _vctPlayers.clear();
    _vctGiocateData.clear();
    _vctPlayerActions.clear();
    _nextActionIndex = 0;
    _hasPendingAction = false;
}

LPErrInApp GameActionReplayer::LoadFile(const std::string& filePath) {
    Reset();
    if (filePath.empty()) {
        return ERR_UTIL::ErrorCreate("GameActionReplayer file path is empty");
    }

    SDL_IOStream* input = SDL_IOFromFile(filePath.c_str(), "rb");
    if (input == NULL) {
        return ERR_UTIL::ErrorCreate("Unable to open replay file %s",
                                     SDL_GetError());
    }

    const Sint64 fileSize = SDL_GetIOSize(input);
    if (fileSize < 0 || static_cast<Uint64>(fileSize) >
                            static_cast<Uint64>(std::numeric_limits<size_t>::max())) {
        SDL_CloseIO(input);
        return ERR_UTIL::ErrorCreate("Unable to read replay file size: %s",
                                     SDL_GetError());
    }

    std::string json(static_cast<size_t>(fileSize), '\0');
    if (!json.empty() && SDL_ReadIO(input, &json[0], json.size()) != json.size()) {
        const std::string readError = SDL_GetError();
        SDL_CloseIO(input);
        return ERR_UTIL::ErrorCreate("Unable to read replay file: %s",
                                     readError.c_str());
    }
    if (!SDL_CloseIO(input)) {
        return ERR_UTIL::ErrorCreate("Unable to close replay file: %s",
                                     SDL_GetError());
    }
    return LoadJson(json);
}

LPErrInApp GameActionReplayer::LoadJson(const std::string& json) {
    const invjson::ParseResult parsed = invjson::Parse(json);
    if (!parsed.success) {
        char errorText[256];
        SDL_snprintf(errorText, sizeof(errorText), "parse error at byte %llu: %s",
                     static_cast<unsigned long long>(parsed.errorOffset),
                     parsed.errorMessage.c_str());
        return InvalidRecording(errorText);
    }

    std::vector<PlayerInfo> playersToLoad;
    std::vector<GiocataData> giocateDataToLoad;
    std::vector<PlayerAction> actionsToLoad;
    const invjson::Value* version = parsed.value.Find("FormatVersion");
    int formatVersion = 0;
    if (!ReadInteger(version, formatVersion) ||
        formatVersion != kSupportedFormatVersion) {
        return InvalidRecording("unsupported or missing FormatVersion");
    }

    const invjson::Value* players = parsed.value.Find("Players");
    if (players == NULL || players->GetType() != invjson::Value::Type::Array ||
        players->AsArray().empty()) {
        return InvalidRecording("Players must be a non-empty array");
    }

    const invjson::Value::Array& playerEntries = players->AsArray();
    for (size_t i = 0; i < playerEntries.size(); ++i) {
        const invjson::Value& entry = playerEntries[i];
        PlayerInfo playerInfo;
        int playerIndex = 0;
        if (!ReadString(entry.Find("Name"), playerInfo.name) ||
            !ReadInteger(entry.Find("Ix"), playerIndex) ||
            playerIndex >= static_cast<int>(NOT_VALID_INDEX)) {
            return InvalidRecording("invalid player metadata");
        }
        playerInfo.index = static_cast<Uint8>(playerIndex);
        if (playerInfo.index != i) {
            return InvalidRecording("player indexes must be contiguous from zero");
        }
        playersToLoad.push_back(playerInfo);
    }

    const invjson::Value* actions = parsed.value.Find("Actions");
    if (actions == NULL || actions->GetType() != invjson::Value::Type::Array) {
        Reset();
        return InvalidRecording("Actions must be an array");
    }

    bool hasMatchStartPlayer = false;
    Uint8 matchStartPlayerIndex = NOT_VALID_INDEX;
    bool hasPendingGiocataStart = false;
    Uint8 pendingGiocataFirstPlayer = NOT_VALID_INDEX;
    const invjson::Value::Array& actionEntries = actions->AsArray();
    for (size_t i = 0; i < actionEntries.size(); ++i) {
        const invjson::Value& entry = actionEntries[i];
        std::string actionName;
        std::string source;
        if (!ReadString(entry.Find("Action"), actionName) ||
            !ReadString(entry.Find("Source"), source)) {
            Reset();
            return InvalidRecording("action entry is missing Action or Source");
        }

        if (source == "Engine" && actionName == "MatchStarted") {
            int playerIndex = 0;
            if (hasMatchStartPlayer ||
                !ReadInteger(entry.Find("Player"), playerIndex) ||
                playerIndex >= static_cast<int>(playersToLoad.size())) {
                Reset();
                return InvalidRecording("MatchStarted has invalid Player index");
            }
            matchStartPlayerIndex = static_cast<Uint8>(playerIndex);
            hasMatchStartPlayer = true;
            continue;
        }

        if (source == "Giocata" && actionName == "EndedWithoutWinner") {
            continue;
        }

        if (source == "Giocata" && actionName == "Started") {
            int playerIndex = 0;
            if (!ReadInteger(entry.Find("Player"), playerIndex) ||
                playerIndex >= static_cast<int>(playersToLoad.size())) {
                Reset();
                return InvalidRecording("Giocata Started has invalid Player index");
            }
            if (hasPendingGiocataStart) {
                Reset();
                return InvalidRecording(
                    "Giocata Started occurred before the previous deck was recorded");
            }
            pendingGiocataFirstPlayer = static_cast<Uint8>(playerIndex);
            hasPendingGiocataStart = true;
            continue;
        }

        if (source == "Engine" && actionName == "ShuffledDeck") {
            if (!hasPendingGiocataStart) {
                Reset();
                return InvalidRecording("ShuffledDeck has no preceding Giocata Started");
            }
            const invjson::Value* deck = entry.Find("Mazzo");
            if (deck == NULL || deck->GetType() != invjson::Value::Type::Array) {
                Reset();
                return InvalidRecording("ShuffledDeck is missing Mazzo array");
            }
            GiocataData giocataData;
            for (size_t cardPosition = 0;
                 cardPosition < deck->AsArray().size(); ++cardPosition) {
                int cardIndex = 0;
                if (!ReadInteger(&deck->AsArray()[cardPosition], cardIndex) ||
                    cardIndex >= NUM_CARDS_MAZZBRI) {
                    Reset();
                    return InvalidRecording("ShuffledDeck has invalid card index");
                }
                giocataData.deckOrder.push_back(
                    static_cast<Uint8>(cardIndex));
            }
            if (deck->AsArray().size() != NUM_CARDS) {
                Reset();
                return InvalidRecording("ShuffledDeck must contain the Invido deck");
            }
            giocataData.firstPlayerIndex = pendingGiocataFirstPlayer;
            giocateDataToLoad.push_back(giocataData);
            hasPendingGiocataStart = false;
            pendingGiocataFirstPlayer = NOT_VALID_INDEX;
            continue;
        }

        if (source != "Player" ||
            (actionName != "Say" && actionName != "PlayCard")) {
            continue;
        }

        int playerIndex = 0;
        int value = 0;
        if (!ReadInteger(entry.Find("Player"), playerIndex) ||
            playerIndex >= static_cast<int>(playersToLoad.size())) {
            Reset();
            return InvalidRecording("player action has an invalid Player index");
        }
        const char* valueKey = actionName == "Say" ? "Say" : "Card";
        if (!ReadInteger(entry.Find(valueKey), value)) {
            Reset();
            return InvalidRecording("player action has an invalid value");
        }
        if (actionName == "Say" && value > SP_NOTHING) {
            Reset();
            return InvalidRecording("Say action value is outside the enum range");
        }
        if (actionName == "PlayCard" && value >= NUM_CARDS_MAZZBRI) {
            Reset();
            return InvalidRecording("PlayCard index is outside the deck range");
        }

        PlayerAction playerAction;
        playerAction.type = actionName == "Say" ? PlayerAction::Type::Say
                                                  : PlayerAction::Type::PlayCard;
        playerAction.playerIndex = static_cast<Uint8>(playerIndex);
        playerAction.value = value;
        playerAction.sequence = i;
        actionsToLoad.push_back(playerAction);
    }

    if (hasPendingGiocataStart) {
        Reset();
        return InvalidRecording("Giocata Started has no following ShuffledDeck");
    }

    _vctPlayers.swap(playersToLoad);
    _vctGiocateData.swap(giocateDataToLoad);
    _vctPlayerActions.swap(actionsToLoad);
    _hasMatchStartPlayer = hasMatchStartPlayer;
    _matchStartPlayerIndex = matchStartPlayerIndex;
    _nextGiocataIndex = 0;
    _nextActionIndex = 0;
    _hasPendingAction = false;
    _isLoaded = true;
    return NULL;
}

size_t GameActionReplayer::GetPlayerCount() const { return _vctPlayers.size(); }

LPErrInApp GameActionReplayer::GetPlayerInfo(Uint8 playerIndex,
                                             PlayerInfo& playerInfo) const {
    if (!_isLoaded) {
        return ERR_UTIL::ErrorCreate("GameActionReplayer is not loaded");
    }
    if (playerIndex >= _vctPlayers.size()) {
        return ERR_UTIL::ErrorCreate("Replay player index %d is out of range",
                                     static_cast<int>(playerIndex));
    }
    playerInfo = _vctPlayers[playerIndex];
    return NULL;
}

LPErrInApp GameActionReplayer::GetMatchStartPlayer(Uint8& playerIndex) const {
    if (!_isLoaded) {
        return ERR_UTIL::ErrorCreate("GameActionReplayer is not loaded");
    }
    if (!_hasMatchStartPlayer) {
        return ERR_UTIL::ErrorCreate("Replay has no MatchStarted player");
    }
    playerIndex = _matchStartPlayerIndex;
    return NULL;
}

LPErrInApp GameActionReplayer::GetNextGiocataData(
    GiocataData& giocataData) {
    if (!_isLoaded) {
        return ERR_UTIL::ErrorCreate("GameActionReplayer is not loaded");
    }
    if (_nextGiocataIndex >= _vctGiocateData.size()) {
        return ERR_UTIL::ErrorCreate("Replay has no remaining shuffled deck");
    }
    giocataData = _vctGiocateData[_nextGiocataIndex];
    ++_nextGiocataIndex;
    return NULL;
}

LPErrInApp GameActionReplayer::PeekNextPlayerActionType(
    Uint8 playerIndex, PlayerAction::Type& actionType, bool& available) const {
    if (!_isLoaded) {
        return ERR_UTIL::ErrorCreate("GameActionReplayer is not loaded");
    }
    if (playerIndex >= _vctPlayers.size()) {
        return ERR_UTIL::ErrorCreate("Replay player index %d is out of range",
                                     static_cast<int>(playerIndex));
    }
    if (_nextActionIndex >= _vctPlayerActions.size()) {
        available = false;
        return NULL;
    }

    const PlayerAction& nextAction = _vctPlayerActions[_nextActionIndex];
    if (nextAction.playerIndex != playerIndex) {
        char errorMessage[128];
        SDL_snprintf(errorMessage, sizeof(errorMessage),
                     "Replay expects player %d before player %d",
                     static_cast<int>(nextAction.playerIndex),
                     static_cast<int>(playerIndex));
        return ERR_UTIL::ErrorCreate(errorMessage);
    }
    actionType = nextAction.type;
    available = true;
    return NULL;
}

LPErrInApp GameActionReplayer::PeekNextPlayerAction(Uint8 playerIndex,
                                                    PlayerAction& action,
                                                    bool& available) {
    if (!_isLoaded) {
        return ERR_UTIL::ErrorCreate("GameActionReplayer is not loaded");
    }
    if (playerIndex >= _vctPlayers.size()) {
        return ERR_UTIL::ErrorCreate("Replay player index %d is out of range",
                                     static_cast<int>(playerIndex));
    }
    if (_hasPendingAction) {
        return ERR_UTIL::ErrorCreate("A replay action is already pending");
    }

    if (_nextActionIndex >= _vctPlayerActions.size()) {
        available = false;
        return NULL;
    }

    const PlayerAction& nextAction = _vctPlayerActions[_nextActionIndex];
    if (nextAction.playerIndex != playerIndex) {
        char errorMessage[128];
        SDL_snprintf(errorMessage, sizeof(errorMessage),
                     "Replay expects player %d before player %d",
                     static_cast<int>(nextAction.playerIndex),
                     static_cast<int>(playerIndex));
        return ERR_UTIL::ErrorCreate(errorMessage);
    }

    available = true;
    action = nextAction;
    _hasPendingAction = true;
    return NULL;
}

bool GameActionReplayer::IsComplete() const {
    return _isLoaded && !_hasPendingAction &&
           _nextActionIndex >= _vctPlayerActions.size();
}

LPErrInApp GameActionReplayer::ConsumeNextPlayerAction(
    const PlayerAction& action) {
    if (!_isLoaded || !_hasPendingAction ||
        _nextActionIndex >= _vctPlayerActions.size()) {
        return ERR_UTIL::ErrorCreate("No replay action is pending");
    }

    const PlayerAction& expected = _vctPlayerActions[_nextActionIndex];
    if (action.sequence != expected.sequence ||
        action.playerIndex != expected.playerIndex ||
        action.type != expected.type || action.value != expected.value) {
        return ERR_UTIL::ErrorCreate("Replay action does not match pending input");
    }

    ++_nextActionIndex;
    _hasPendingAction = false;
    return NULL;
}

}  // namespace invido
