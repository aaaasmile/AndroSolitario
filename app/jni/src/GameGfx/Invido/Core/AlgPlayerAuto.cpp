#include "AlgPlayerAuto.h"

#include <SDL3/SDL.h>

namespace invido {

AlgPlayerAuto::AlgPlayerAuto(GameActionReplayer& replayer, Uint8 playerIndex)
    : _replayer(replayer), _p_CoreGame(NULL), _playerIndex(playerIndex) {}

AlgPlayerAuto::~AlgPlayerAuto() {}

void AlgPlayerAuto::ALG_SetCoreInterface(I_CORE_Game* core) {
    _p_CoreGame = core;
}

void AlgPlayerAuto::ALG_SetPlayerIndex(Uint8 playerIndex) {
    _playerIndex = playerIndex;
}

void AlgPlayerAuto::ALG_SetOpponentIndex(Uint8 playerIndex) {}
void AlgPlayerAuto::ALG_SetAssociateIndex(Uint8 playerIndex) {}
void AlgPlayerAuto::ALG_NewMatch(int playerCount) {}
void AlgPlayerAuto::ALG_NewGiocata(const VCT_CARDSPEC& cards,
                                   Uint8 playerIndex) {}
void AlgPlayerAuto::ALG_PlayerHasPlayed(Uint8 playerIndex,
                                        const CardSpec& card) {}
void AlgPlayerAuto::ALG_PlayerHasVadoDentro(Uint8 playerIndex) {}
void AlgPlayerAuto::ALG_PlayerHasSaid(Uint8 playerIndex, eSayPlayer say) {}
void AlgPlayerAuto::ALG_ManoEnd(I_MatchScore* score) {}
void AlgPlayerAuto::ALG_GiocataEnd(I_MatchScore* score) {}
void AlgPlayerAuto::ALG_MatchEnd(I_MatchScore* score) {}
void AlgPlayerAuto::ALG_GicataScoreChange(eGiocataScoreState newScore) {}
void AlgPlayerAuto::ALG_PlayerSaidWrong(Uint8 playerIndex) {}

LPErrInApp AlgPlayerAuto::ALG_HaveToPlay() {
    GameActionReplayer::PlayerAction::Type actionType;
    bool available = false;
    LPErrInApp error =
        _replayer.PeekNextPlayerActionType(_playerIndex, actionType, available);
    if (error != NULL) {
        return error;
    }
    if (!available) {
        return ERR_UTIL::ErrorCreate(
            "Replay has no remaining action for player %d",
            static_cast<int>(_playerIndex));
    }
    return PlayNextRecordedAction(actionType);
}

LPErrInApp AlgPlayerAuto::ALG_HaveToRespond() {
    return PlayNextRecordedAction(GameActionReplayer::PlayerAction::Type::Say);
}

LPErrInApp AlgPlayerAuto::PlayNextRecordedAction(
    GameActionReplayer::PlayerAction::Type expectedType) {
    SDL_assert(_p_CoreGame);

    GameActionReplayer::PlayerAction action;
    bool available = false;
    LPErrInApp error =
        _replayer.PeekNextPlayerAction(_playerIndex, action, available);
    if (error != NULL) {
        return error;
    }
    if (!available) {
        char errorMessage[128];
        SDL_snprintf(errorMessage, sizeof(errorMessage),
                     "Replay has no remaining action for player %d",
                     static_cast<int>(_playerIndex));
        return ERR_UTIL::ErrorCreate(errorMessage);
    }
    if (action.type != expectedType) {
        char errorMessage[128];
        SDL_snprintf(errorMessage, sizeof(errorMessage),
                     "Replay action %d has the wrong type for player %d",
                     static_cast<int>(action.sequence),
                     static_cast<int>(_playerIndex));
        return ERR_UTIL::ErrorCreate(errorMessage);
    }

    bool accepted = false;
    switch (expectedType) {
        case GameActionReplayer::PlayerAction::Type::Say:
            accepted = _p_CoreGame->Say(_playerIndex,
                                        static_cast<eSayPlayer>(action.value));
            break;
        case GameActionReplayer::PlayerAction::Type::PlayCard:
        case GameActionReplayer::PlayerAction::Type::VaDentro: {
            CardSpec card;
            card.SetCardIndex(static_cast<Uint8>(action.value));
            if (expectedType ==
                GameActionReplayer::PlayerAction::Type::VaDentro) {
                accepted = _p_CoreGame->VaDentro(_playerIndex, card);
            } else {
                accepted = _p_CoreGame->PlayCard(_playerIndex, card);
            }
            break;
        }
    }

    if (!accepted) {
        const char* actionName = "";
        switch (expectedType) {
            case GameActionReplayer::PlayerAction::Type::Say:
                actionName = "Say";
                break;
            case GameActionReplayer::PlayerAction::Type::PlayCard:
                actionName = "PlayCard";
                break;
            case GameActionReplayer::PlayerAction::Type::VaDentro:
                actionName = "VaDentro";
                break;
        }
        char errorMessage[128];
        SDL_snprintf(errorMessage, sizeof(errorMessage),
                     "Replay %s action %d was rejected for player %d",
                     actionName, static_cast<int>(action.sequence),
                     static_cast<int>(_playerIndex));
        return ERR_UTIL::ErrorCreate(errorMessage);
    }
    return _replayer.ConsumeNextPlayerAction(action);
}

}  // namespace invido
