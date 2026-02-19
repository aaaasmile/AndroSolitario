#include <cassert>
#include <iostream>

#include "../Player.h"
#include "../PlayersOnTable.h"

void testPlayersOnTableCreation() {
    std::cout << "Running PlayersOnTable creation tests..." << std::endl;

    invido::PlayersOnTable players;

    assert(players.GetFirstOnMatch() == 0);
    assert(players.GetFirstOnTrick() == 0);
    assert(players.GetFirstOnGiocata() == 0);

    std::cout << "PlayersOnTable creation tests PASSED" << std::endl;
}

void testPlayersOnTableSetFirst() {
    std::cout << "Running PlayersOnTable set first tests..." << std::endl;

    invido::PlayersOnTable playersOnTable;
    invido::Player localPlayer;
    localPlayer.Create();
    playersOnTable.Create(&localPlayer, 2);
    playersOnTable.SetFirstOnMatch(0);

    assert(playersOnTable.GetFirstOnMatch() == 0);

    playersOnTable.SetFirstOnTrick(0);
    assert(playersOnTable.GetFirstOnTrick() == 0);

    playersOnTable.SetFirstOnGiocata(1);
    assert(playersOnTable.GetFirstOnGiocata() == 1);

    std::cout << "PlayersOnTable set first tests PASSED" << std::endl;
}

void testPlayersOnTableCreatePlayers() {
    std::cout << "Running PlayersOnTable create players tests..." << std::endl;

    invido::PlayersOnTable players;
    invido::Player localPlayer;
    localPlayer.Create();
    localPlayer.SetType(invido::PT_LOCAL);

    players.Create(&localPlayer, 2);

    invido::Player* player0 = players.GetPlayerIndex(0);
    assert(player0 != nullptr);
    assert(player0->GetType() == invido::PT_LOCAL);

    invido::Player* player1 = players.GetPlayerIndex(1);
    assert(player1 != nullptr);
    assert(player1->GetType() == invido::PT_MACHINE);

    std::cout << "PlayersOnTable create players tests PASSED" << std::endl;
}

void testPlayersOnTableCalcCircleIndex() {
    std::cout << "Running PlayersOnTable calc circle index tests..."
              << std::endl;

    invido::PlayersOnTable players;
    invido::Player localPlayer;
    localPlayer.Create();

    players.Create(&localPlayer, 4);
    players.SetFirstOnTrick(0);

    int indices[4];
    players.CalcCircleIndex(indices);

    assert(indices[0] == 0);
    assert(indices[1] == 1);
    assert(indices[2] == 2);
    assert(indices[3] == 3);

    std::cout << "PlayersOnTable calc circle index tests PASSED" << std::endl;
}

void testPlayersOnTable() {
    testPlayersOnTableCreation();
    testPlayersOnTableSetFirst();
    testPlayersOnTableCreatePlayers();
    testPlayersOnTableCalcCircleIndex();
}
