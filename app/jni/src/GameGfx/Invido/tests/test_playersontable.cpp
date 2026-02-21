#include <cassert>
#include <iostream>

#include "../Player.h"
#include "../PlayersOnTable.h"

void testPlayersOnTableCreation() {
    std::cout << "Running PlayersOnTable creation tests..." << std::endl;

    invido::PlayersOnTable playersOnTable;

    assert(playersOnTable.GetFirstOnMatch() == 0);
    assert(playersOnTable.GetFirstOnTrick() == 0);
    assert(playersOnTable.GetFirstOnGiocata() == 0);

    std::cout << "PlayersOnTable creation tests PASSED" << std::endl;
}

void testPlayersOnTableSetFirst() {
    std::cout << "Running PlayersOnTable set first tests..." << std::endl;

    invido::PlayersOnTable playersOnTable;
    invido::Player localPlayerOne;
    invido::Player localPlayerTwo;
    localPlayerOne.Init(NULL, invido::PT_LOCAL, "Guido", 0);
    localPlayerTwo.Init(NULL, invido::PT_MACHINE, "Spatasa", 2);

    playersOnTable.AddPlayer(localPlayerOne);
    playersOnTable.AddPlayer(localPlayerTwo);
    playersOnTable.SetFirstOnMatch(0);

    assert(playersOnTable.GetFirstOnMatch() == 0);

    playersOnTable.SetCurrentAndFirstOnTrick(0);
    assert(playersOnTable.GetFirstOnTrick() == 0);

    playersOnTable.SetFirstOnGiocata(1);
    assert(playersOnTable.GetFirstOnGiocata() == 1);

    std::cout << "PlayersOnTable set first tests PASSED" << std::endl;
}

void testPlayersOnTableCreatePlayers() {
    std::cout << "Running PlayersOnTable create players tests..." << std::endl;

    invido::PlayersOnTable playersOnTable;
    
    invido::Player localPlayerOne;
    invido::Player localPlayerTwo;
    localPlayerOne.Init(NULL, invido::PT_LOCAL, "Guido", 0);
    localPlayerTwo.Init(NULL, invido::PT_MACHINE, "Spatasa", 2);
    
    playersOnTable.AddPlayer(localPlayerOne);
    playersOnTable.AddPlayer(localPlayerTwo);

    invido::Player* player0 = playersOnTable.GetPlayerOnIndex(0);
    assert(player0 != nullptr);
    assert(player0->GetType() == invido::PT_LOCAL);

    invido::Player* player1 = playersOnTable.GetPlayerOnIndex(1);
    assert(player1 != nullptr);
    assert(player1->GetType() == invido::PT_MACHINE);

    std::cout << "PlayersOnTable create players tests PASSED" << std::endl;
}

void testPlayersOnTableCalcCircleIndex() {
    std::cout << "Running PlayersOnTable calc circle index tests..."
              << std::endl;

    invido::PlayersOnTable playerOnTable;
    invido::Player playerOne;
    invido::Player playerTwo;
    invido::Player playerThree;
    invido::Player playerFour;
    playerOne.Init(NULL, invido::PT_MACHINE, "Guido", 0);
    playerTwo.Init(NULL, invido::PT_MACHINE, "Guido2", 1);
    playerThree.Init(NULL, invido::PT_MACHINE, "Guido3", 3);
    playerFour.Init(NULL, invido::PT_MACHINE, "Guido4", 4);

    playerOnTable.SetCurrentAndFirstOnTrick(0);

    int indices[4];
    playerOnTable.CalcCircleIndex(indices, 4);

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
