#ifndef INVIDO_GAME_ACTION_REPLAYER_H
#define INVIDO_GAME_ACTION_REPLAYER_H

#include <cstddef>
#include <string>
#include <vector>

#include "../../../Lib/Json/Json.h"
#include "ErrorInfo.h"
#include "InvidoCoreDef.h"

namespace invido {

class GameActionReplayer {
   public:
    struct PlayerAction {
        enum class Type { Say, PlayCard };

        PlayerAction()
            : type(Type::Say),
              playerIndex(NOT_VALID_INDEX),
              value(0),
              sequence(0) {}

        Type type;
        Uint8 playerIndex;
        int value;
        size_t sequence;
    };

    struct PlayerInfo {
        std::string name;
        Uint8 index;
    };

    struct GiocataData {
        Uint8 firstPlayerIndex;
        std::vector<Uint8> deckOrder;
    };

    GameActionReplayer();

    LPErrInApp LoadFile(const std::string& filePath);
    size_t GetPlayerCount() const;
    LPErrInApp GetPlayerInfo(Uint8 playerIndex, PlayerInfo& playerInfo) const;
    LPErrInApp GetMatchStartPlayer(Uint8& playerIndex) const;
    LPErrInApp GetNextGiocataData(GiocataData& giocataData);
    LPErrInApp PeekNextPlayerAction(Uint8 playerIndex, PlayerAction& action,
                                   bool& available);
    LPErrInApp PeekNextPlayerActionType(Uint8 playerIndex,
                                        PlayerAction::Type& actionType,
                                        bool& available) const;
    LPErrInApp ConsumeNextPlayerAction(const PlayerAction& action);
    bool IsComplete() const;

   private:
    LPErrInApp LoadJson(const std::string& json);
    void Reset();

    bool _isLoaded;
    bool _hasMatchStartPlayer;
    Uint8 _matchStartPlayerIndex;
    size_t _nextGiocataIndex;
    std::vector<PlayerInfo> _vctPlayers;
    std::vector<GiocataData> _vctGiocateData;
    std::vector<PlayerAction> _vctPlayerActions;
    size_t _nextActionIndex;
    bool _hasPendingAction;
};

}  // namespace invido

#endif
