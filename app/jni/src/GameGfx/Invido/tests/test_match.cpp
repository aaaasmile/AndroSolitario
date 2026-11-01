#include <iostream>
#include <stdexcept>

#include "../Core/InvidoCore.h"
#include "../Core/Mazzo.h"
#include "../Core/Player.h"
#include "../Core/PlayersOnTable.h"

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

void testMatch() { testSimpleMatch(); }