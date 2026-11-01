#include <iostream>
#include <stdexcept>
#include <cassert>

#include "../Core/InvidoCore.h"
#include "../Core/Mazzo.h"
#include "../Core/Player.h"
#include "../Core/PlayersOnTable.h"
#include "../Core/GameActionReplayer.h"

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
        if (!invidoCore.NextAction()) {
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

    mazzo.Init();

    const std::string path = "..\\matches\\Invido-match-1511400.json_ref";

    invido::GameActionReplayer replayer;
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

    std::string jsonDir = "matches";
    invidoCore.SetRecordingLocation(jsonDir, "Invido");

    invidoCore.Init(&playersOnTable, &mazzo);
    invidoCore.NewMatch();
    while (invidoCore.IsMatchOnGoing()) {
        if (!invidoCore.NextAction()) {
            throw std::runtime_error("Match stalled: no game action was taken");
        }
    }

    std::cout << "Match replay tests PASSED" << std::endl;
}

void testMatch() { 
    testReplayMatch();
    testSimpleMatch(); 
}