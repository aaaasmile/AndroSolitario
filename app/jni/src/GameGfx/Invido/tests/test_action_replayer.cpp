#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "../../../Lib/Json/Json.h"
#include "../Core/AlgPlayerAuto.h"
#include "../Core/GameActionReplayer.h"
#include "../Core/Player.h"
#include "../Core/InvidoCoreDef.h"

namespace {

struct CoreCommand {
    std::string name;
    Uint8 playerIndex;
    int value;
};

class FakeCore : public invido::I_CORE_Game {
   public:
    explicit FakeCore(bool acceptCommands = true)
        : _acceptCommands(acceptCommands) {}

    virtual bool Say(Uint8 playerIndex, invido::eSayPlayer say) {
        CoreCommand command = {"Say", playerIndex, static_cast<int>(say)};
        _commands.push_back(command);
        return _acceptCommands;
    }

    virtual bool PlayCard(Uint8 playerIndex, const invido::CardSpec& card) {
        CoreCommand command = {"PlayCard", playerIndex, card.GetCardIndex()};
        _commands.push_back(command);
        return _acceptCommands;
    }

    virtual bool VaDentro(Uint8 playerIndex, const invido::CardSpec& card) {
        CoreCommand command = {"VaDentro", playerIndex, card.GetCardIndex()};
        _commands.push_back(command);
        return _acceptCommands;
    }

    virtual void AbandonGame(Uint8 playerIndex) {
        CoreCommand command = {"Abandon", playerIndex, -1};
        _commands.push_back(command);
    }

    virtual void GetAdmittedCommands(invido::VCT_COMMANDS& commands,
                                     Uint8 playerIndex) {
        commands.clear();
    }

    virtual void GetMoreCommands(invido::VCT_COMMANDS& commands,
                                 Uint8 playerIndex) {
        commands.clear();
    }

    const std::vector<CoreCommand>& Commands() const { return _commands; }

   private:
    bool _acceptCommands;
    std::vector<CoreCommand> _commands;
};

invjson::Value MakeAction(const char* source, const char* action, int player,
                          const char* valueKey, int value) {
    invjson::Value entry = invjson::Value::MakeObject();
    entry["Source"] = invjson::Value(source);
    entry["Action"] = invjson::Value(action);
    if (player >= 0) {
        entry["Player"] = invjson::Value(player);
    }
    if (valueKey != NULL) {
        entry[valueKey] = invjson::Value(value);
    }
    return entry;
}

invjson::Value MakeRecording() {
    invjson::Value recording = invjson::Value::MakeObject();
    recording["FormatVersion"] = invjson::Value(1);

    invjson::Value players = invjson::Value::MakeArray();
    invjson::Value firstPlayer = invjson::Value::MakeObject();
    firstPlayer["Name"] = invjson::Value("Rensu \"Ace\"");
    firstPlayer["Ix"] = invjson::Value(0);
    players.PushBack(firstPlayer);
    invjson::Value secondPlayer = invjson::Value::MakeObject();
    secondPlayer["Name"] = invjson::Value("Guido");
    secondPlayer["Ix"] = invjson::Value(1);
    players.PushBack(secondPlayer);
    recording["Players"] = players;

    invjson::Value actions = invjson::Value::MakeArray();
    actions.PushBack(MakeAction("Engine", "MatchStarted", 0, NULL, 0));
    actions.PushBack(MakeAction("Engine", "Say", -1, "Say",
                                invido::SP_PARTIDA));
    actions.PushBack(MakeAction("Player", "Say", 0, "Say",
                                invido::SP_INVIDO));
    actions.PushBack(MakeAction("Engine", "Deal", -1, NULL, 0));
    actions.PushBack(MakeAction("Player", "Say", 1, "Say",
                                invido::SP_VABENE));
    actions.PushBack(MakeAction("Giocata", "Started", 0, NULL, 0));
    invjson::Value shuffledDeck = MakeAction("Engine", "ShuffledDeck", -1, NULL, 0);
    invjson::Value deck = invjson::Value::MakeArray();
    const int cardOrder[] = {27, 16, 15, 12, 32, 0, 10, 8, 25, 37, 36, 18,
                             17, 1, 30, 31, 29, 39, 38, 26, 9, 35, 20, 22,
                             2, 6, 11, 19, 28, 5, 21, 7};
    for (size_t card = 0; card < sizeof(cardOrder) / sizeof(cardOrder[0]); ++card) {
        deck.PushBack(invjson::Value(cardOrder[card]));
    }
    shuffledDeck["Mazzo"] = deck;
    actions.PushBack(shuffledDeck);
    actions.PushBack(MakeAction("Player", "PlayCard", 0, "Card", 5));
    actions.PushBack(MakeAction("Player", "PlayCard", 1, "Card", 12));
    recording["Actions"] = actions;
    return recording;
}

bool WriteJsonFile(const std::string& path, const invjson::Value& value) {
    const std::string json = invjson::Stringify(value);
    SDL_IOStream* output = SDL_IOFromFile(path.c_str(), "wb");
    if (output == NULL) {
        return false;
    }
    const bool writeSucceeded =
        SDL_WriteIO(output, json.data(), json.size()) == json.size();
    return SDL_CloseIO(output) && writeSucceeded;
}

}  // namespace

void testGameActionReplayer() {
    std::cout << "Running GameActionReplayer tests..." << std::endl;

    const std::string path = "GameActionReplayerTest.json";
    const invjson::Value recording = MakeRecording();
    assert(WriteJsonFile(path, recording));

    invido::GameActionReplayer replayer;
    LPErrInApp error = replayer.LoadFile(path);
    assert(error == NULL);
    assert(replayer.GetPlayerCount() == 2);

    invido::GameActionReplayer::PlayerInfo playerInfo;
    error = replayer.GetPlayerInfo(0, playerInfo);
    assert(error == NULL);
    assert(playerInfo.name == "Rensu \"Ace\"");
    assert(playerInfo.index == 0);

    Uint8 matchStartPlayer = invido::NOT_VALID_INDEX;
    error = replayer.GetMatchStartPlayer(matchStartPlayer);
    assert(error == NULL);
    assert(matchStartPlayer == 0);

    invido::GameActionReplayer::GiocataData giocataData;
    error = replayer.GetNextGiocataData(giocataData);
    assert(error == NULL);
    assert(giocataData.firstPlayerIndex == 0);
    assert(giocataData.deckOrder.size() == 32);
    assert(giocataData.deckOrder.front() == 27);
    assert(giocataData.deckOrder.back() == 7);

    invido::GameActionReplayer::PlayerAction nextAction;
    bool available = false;
    error = replayer.PeekNextPlayerAction(1, nextAction, available);
    assert(error != NULL);
    assert(!available);

    invido::Player player0;
    invido::Player player1;
    error = player0.InitFromActionPlayer(replayer, 0);
    assert(error == NULL);
    error = player1.InitFromActionPlayer(replayer, 1);
    assert(error == NULL);
    assert(player0.GetName() == "Rensu \"Ace\"");
    assert(player1.GetName() == "Guido");
    assert(dynamic_cast<invido::AlgPlayerAuto*>(player0.GetAlg()) != NULL);
    assert(dynamic_cast<invido::AlgPlayerAuto*>(player1.GetAlg()) != NULL);

    invido::GameActionReplayer flexibleTurnReplayer;
    error = flexibleTurnReplayer.LoadFile(path);
    assert(error == NULL);
    invido::AlgPlayerAuto flexibleTurnPlayer(flexibleTurnReplayer, 0);
    FakeCore flexibleTurnCore;
    flexibleTurnPlayer.ALG_SetCoreInterface(&flexibleTurnCore);
    error = flexibleTurnPlayer.ALG_HaveToPlay();
    assert(error == NULL);
    assert(flexibleTurnCore.Commands().size() == 1);
    assert(flexibleTurnCore.Commands()[0].name == "Say");
    assert(!flexibleTurnReplayer.IsComplete());

    invjson::Value responseMismatchRecording = MakeRecording();
    invjson::Value responseMismatchActions = invjson::Value::MakeArray();
    responseMismatchActions.PushBack(
        MakeAction("Player", "PlayCard", 0, "Card", 5));
    responseMismatchRecording["Actions"] = responseMismatchActions;
    const std::string responseMismatchPath =
        "GameActionReplayerResponseMismatchTest.json";
    assert(WriteJsonFile(responseMismatchPath, responseMismatchRecording));
    invido::GameActionReplayer responseMismatchReplayer;
    error = responseMismatchReplayer.LoadFile(responseMismatchPath);
    assert(error == NULL);
    invido::AlgPlayerAuto wrongResponsePlayer(responseMismatchReplayer, 0);
    FakeCore wrongResponseCore;
    wrongResponsePlayer.ALG_SetCoreInterface(&wrongResponseCore);
    error = wrongResponsePlayer.ALG_HaveToRespond();
    assert(error == NULL);
    assert(wrongResponseCore.Commands().empty());
    assert(!responseMismatchReplayer.IsComplete());

    FakeCore core;
    player0.GetAlg()->ALG_SetCoreInterface(&core);
    player1.GetAlg()->ALG_SetCoreInterface(&core);
    player0.GetAlg()->ALG_HaveToRespond();
    player1.GetAlg()->ALG_HaveToRespond();
    player0.GetAlg()->ALG_HaveToPlay();
    player1.GetAlg()->ALG_HaveToPlay();

    const std::vector<CoreCommand>& commands = core.Commands();
    assert(commands.size() == 4);
    assert(commands[0].name == "Say" && commands[0].playerIndex == 0 &&
           commands[0].value == invido::SP_INVIDO);
    assert(commands[1].name == "Say" && commands[1].playerIndex == 1 &&
           commands[1].value == invido::SP_VABENE);
    assert(commands[2].name == "PlayCard" && commands[2].playerIndex == 0 &&
           commands[2].value == 5);
    assert(commands[3].name == "PlayCard" && commands[3].playerIndex == 1 &&
           commands[3].value == 12);
    assert(replayer.IsComplete());

    invido::Player copiedPlayer = player0;
    player0.CleanUp();
    assert(player0.GetAlg() == NULL);
    assert(copiedPlayer.GetAlg() != NULL);
    copiedPlayer.CleanUp();

    invjson::Value rejectedRecording = invjson::Value::MakeObject();
    rejectedRecording["FormatVersion"] = invjson::Value(1);
    rejectedRecording["Players"] = *recording.Find("Players");
    invjson::Value rejectedActions = invjson::Value::MakeArray();
    rejectedActions.PushBack(MakeAction("Player", "PlayCard", 0, "Card", 5));
    rejectedRecording["Actions"] = rejectedActions;
    const std::string rejectedPath = "GameActionReplayerRejectedTest.json";
    assert(WriteJsonFile(rejectedPath, rejectedRecording));

    invido::GameActionReplayer rejectedReplayer;
    error = rejectedReplayer.LoadFile(rejectedPath);
    assert(error == NULL);
    invido::AlgPlayerAuto rejectingPlayer(rejectedReplayer, 0);
    FakeCore rejectingCore(false);
    rejectingPlayer.ALG_SetCoreInterface(&rejectingCore);
    error = rejectingPlayer.ALG_HaveToPlay();
    assert(error == NULL);
    assert(!rejectedReplayer.IsComplete());

    invjson::Value unsupportedRecording = MakeRecording();
    unsupportedRecording["FormatVersion"] = invjson::Value(99);
    const std::string unsupportedPath = "GameActionReplayerUnsupportedTest.json";
    assert(WriteJsonFile(unsupportedPath, unsupportedRecording));
    invido::GameActionReplayer unsupportedReplayer;
    error = unsupportedReplayer.LoadFile(unsupportedPath);
    assert(error != NULL);
    assert(unsupportedReplayer.GetPlayerCount() == 0);

    const std::string truncatedPath = "GameActionReplayerTruncatedTest.json";
    invjson::Value truncatedRecording = MakeRecording();
    invjson::Value truncatedActions = invjson::Value::MakeArray();
    truncatedActions.PushBack(
        MakeAction("Player", "Say", 0, "Say", invido::SP_INVIDO));
    truncatedRecording["Actions"] = truncatedActions;
    assert(WriteJsonFile(truncatedPath, truncatedRecording));
    invido::GameActionReplayer truncatedReplayer;
    error = truncatedReplayer.LoadFile(truncatedPath);
    assert(error == NULL);
    invido::AlgPlayerAuto playerWaitingForRecordedSay(truncatedReplayer, 0);
    FakeCore sayCore;
    playerWaitingForRecordedSay.ALG_SetCoreInterface(&sayCore);
    error = playerWaitingForRecordedSay.ALG_HaveToPlay();
    assert(error == NULL);
    assert(sayCore.Commands().size() == 1);
    assert(sayCore.Commands()[0].name == "Say");

    assert(SDL_RemovePath(path.c_str()));
    assert(SDL_RemovePath(responseMismatchPath.c_str()));
    assert(SDL_RemovePath(rejectedPath.c_str()));
    assert(SDL_RemovePath(unsupportedPath.c_str()));
    assert(SDL_RemovePath(truncatedPath.c_str()));
    assert(SDL_RemovePath(truncatedPath.c_str()));
    std::cout << "GameActionReplayer tests PASSED" << std::endl;
}
