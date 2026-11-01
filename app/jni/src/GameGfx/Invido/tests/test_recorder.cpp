#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "../Core/GameActionRecorder.h"
#include "../Core/Player.h"
#include "../Core/PlayersOnTable.h"

void testGameActionRecorder() {
    std::cout << "Running GameActionRecorder tests..." << std::endl;

    invido::Player player0;
    invido::Player player1;
    player0.Init(NULL, invido::PT_MACHINE, "Rensu \"The Ace\"", 0);
    player1.Init(NULL, invido::PT_MACHINE, "Guido", 1);
    invido::PlayersOnTable players;
    players.AddPlayer(player0);
    players.AddPlayer(player1);

    invido::GameActionRecorder recorder;
    const std::string path = "GameActionRecorderTest.json";
    LPErrInApp error = recorder.InitializeAtPath(players, path);
    assert(error == NULL);

    SDL_IOStream* input = SDL_IOFromFile(path.c_str(), "rb");
    assert(input != NULL);
    const Sint64 fileSize = SDL_GetIOSize(input);
    assert(fileSize > 0);
    std::string initialJson(static_cast<size_t>(fileSize), '\0');
    assert(SDL_ReadIO(input, &initialJson[0], initialJson.size()) ==
           initialJson.size());
    assert(SDL_CloseIO(input));
    const invjson::ParseResult initialDocument = invjson::Parse(initialJson);
    assert(initialDocument.success);
    assert(initialDocument.value.Find("Mazzo") == NULL);
    assert(initialDocument.value.Find("Players")->AsArray().size() == 2);
    assert(initialDocument.value.Find("Actions")->AsArray().empty());
    assert(initialDocument.value.Find("Players")
               ->AsArray()[0]
               .Find("Name")
               ->AsString() == "Rensu \"The Ace\"");

    invjson::Value::Object actionDetails;
    actionDetails["Player"] = invjson::Value(1);
    actionDetails["Card"] = invjson::Value(39);
    recorder.RecordAction("Player", "PlayCard", actionDetails);
    assert(recorder.GetActions().size() == 1);

    error = recorder.Finalize("Completed");
    assert(error == NULL);
    error = recorder.Finalize("Completed");
    assert(error == NULL);
    input = SDL_IOFromFile(path.c_str(), "rb");
    assert(input != NULL);
    const Sint64 finalSize = SDL_GetIOSize(input);
    assert(finalSize > 0);
    std::string finalJson(static_cast<size_t>(finalSize), '\0');
    assert(SDL_ReadIO(input, &finalJson[0], finalJson.size()) ==
           finalJson.size());
    assert(SDL_CloseIO(input));
    const invjson::ParseResult finalDocument = invjson::Parse(finalJson);
    assert(finalDocument.success);
    assert(finalDocument.value.Find("Actions")->AsArray().size() == 1);
    assert(finalDocument.value.Find("Outcome")->AsString() == "Completed");

    assert(SDL_RemovePath(path.c_str()));
    std::cout << "GameActionRecorder tests PASSED" << std::endl;
}
