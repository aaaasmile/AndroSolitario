#include <iostream>
#include <stdexcept>
#include <cassert>

#include "../Core/InvidoCore.h"
#include "../Core/Mazzo.h"
#include "../Core/Player.h"
#include "../Core/PlayersOnTable.h"
#include "../Core/GameActionReplayer.h"

void testSayPlayerToString() {
    struct SayPlayerName {
        invido::eSayPlayer value;
        const char* name;
    };
    const SayPlayerName sayPlayerNames[] = {
        {invido::SP_AMONTE, "SP_AMONTE"},
        {invido::SP_INVIDO, "SP_INVIDO"},
        {invido::SP_TRASMAS, "SP_TRASMAS"},
        {invido::SP_TRASMASNOEF, "SP_TRASMASNOEF"},
        {invido::SP_FUERAJEUQ, "SP_FUERAJEUQ"},
        {invido::SP_PARTIDA, "SP_PARTIDA"},
        {invido::SP_VABENE, "SP_VABENE"},
        {invido::SP_VADOVIA, "SP_VADOVIA"},
        {invido::SP_CHIAMADIPIU, "SP_CHIAMADIPIU"},
        {invido::SP_NO, "SP_NO"},
        {invido::SP_VADODENTRO, "SP_VADODENTRO"},
        {invido::SP_GIOCA, "SP_GIOCA"},
        {invido::SP_CHIAMA_BORTOLO, "SP_CHIAMA_BORTOLO"},
        {invido::SP_NOTHING, "SP_NOTHING"},
    };

    for (const SayPlayerName& sayPlayerName : sayPlayerNames) {
        assert(invido::GeteSayPlayerToString(sayPlayerName.value) ==
               std::string(sayPlayerName.name));
    }
}

void testSimpleMatch() {
    std::cout << "Running Match tests..." << std::endl;

    invido::PlayersOnTable playersOnTable;
    invido::Player player1;
    invido::Player player2;
    invido::InvidoCore invidoCore;
    invido::Mazzo mazzo;

    mazzo.Init();
    mazzo.SetRandomSeed(1500);

    player1.Init(NULL, invido::PT_MACHINE, "Guido", 0);
    player2.Init(NULL, invido::PT_MACHINE, "Rensu", 1);
    playersOnTable.AddPlayer(player1);
    playersOnTable.AddPlayer(player2);

    std::string jsonDir = "matches";
    invidoCore.SetRecordingLocation(jsonDir, "Invido");

    invidoCore.Init(&playersOnTable, &mazzo);
    invidoCore.NewMatch();
    while (invidoCore.IsMatchOnGoing()) {
        bool hasAction = false;
        LPErrInApp error = invidoCore.NextAction(hasAction);
        if (error != NULL) {
            throw std::runtime_error(error->ErrorText);
        }
        if (!hasAction) {
            throw std::runtime_error("Match stalled: no game action was taken");
        }
    }

    std::cout << "Match tests PASSED" << std::endl;
}


void testReplayMatch() {
    std::cout << "Running Match replay tests..." << std::endl;

    invido::PlayersOnTable playersOnTable;
    invido::Player player1;
    invido::Player player2;
    invido::InvidoCore invidoCore;
    invido::Mazzo mazzo;
    invido::GameActionReplayer replayer;

    mazzo.Init();

    const std::string path = "..\\matches\\Invido-match-1511400.json_ref";

    LPErrInApp error = replayer.LoadFile(path);
    assert(error == NULL);
    assert(replayer.GetPlayerCount() == 2);

    player1.InitFromActionPlayer(replayer, 0);
    player2.InitFromActionPlayer(replayer, 1);
    assert(player1.GetName() == "Guido");
    assert(player1.GetIndex() == 0);
    assert(player2.GetName() == "Rensu");
    assert(player2.GetIndex() == 1);

    playersOnTable.AddPlayer(player1);
    playersOnTable.AddPlayer(player2);
    invidoCore.SetGameActionReplayer(replayer);

    std::string jsonDir = "matches";
    invidoCore.SetRecordingLocation(jsonDir, "Invido");
    invidoCore.Init(&playersOnTable, &mazzo);
    invidoCore.NewMatch();
    while (invidoCore.IsMatchOnGoing()) {
        bool hasAction = false;
        LPErrInApp error = invidoCore.NextAction(hasAction);
        if (error != NULL) {
            throw std::runtime_error(error->ErrorText);
        }
        if (!hasAction) {
            throw std::runtime_error("Match stalled: no game action was taken");
        }
    }

    std::cout << "Match replay tests PASSED" << std::endl;
}

void testMatch() {
    testSayPlayerToString();
    testReplayMatch();
    testSimpleMatch();
}