#include <cassert>
#include <iostream>

#include "../../../Lib/Json/Json.h"

void testJson() {
    std::cout << "Running JSON parser/writer tests..." << std::endl;

    invjson::Value root = invjson::Value::MakeObject();
    root["Title"] = invjson::Value("Invido \"match\"\nlog");
    root["Score"] = invjson::Value(24);
    root["IsFinal"] = invjson::Value(true);
    root["Empty"] = invjson::Value();

    invjson::Value players = invjson::Value::MakeArray();
    invjson::Value player = invjson::Value::MakeObject();
    player["Name"] = invjson::Value(std::string("Ren\\su\n"));
    player["Ix"] = invjson::Value(1);
    players.PushBack(player);
    root["Players"] = players;

    const std::string serialized = invjson::Stringify(root);
    assert(serialized.find("\\\"match\\\"\\nlog") != std::string::npos);

    const invjson::ParseResult parsed = invjson::Parse(serialized);
    assert(parsed.success);
    assert(parsed.value.Find("Title") != NULL);
    assert(parsed.value.Find("Title")->AsString() == "Invido \"match\"\nlog");
    assert(parsed.value.Find("Score")->AsNumber() == 24.0);
    assert(parsed.value.Find("IsFinal")->AsBoolean());
    assert(parsed.value.Find("Empty")->IsNull());

    const invjson::Value* parsedPlayers = parsed.value.Find("Players");
    assert(parsedPlayers != NULL);
    assert(parsedPlayers->AsArray().size() == 1);
    assert(parsedPlayers->AsArray()[0].Find("Name")->AsString() ==
           "Ren\\su\n");

    const invjson::ParseResult malformed = invjson::Parse("{\"key\": [1,]}");
    assert(!malformed.success);
    assert(!malformed.errorMessage.empty());

    const invjson::ParseResult trailing = invjson::Parse("true false");
    assert(!trailing.success);

    std::cout << "JSON parser/writer tests PASSED" << std::endl;
}
