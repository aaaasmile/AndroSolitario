#include <cassert>
#include <iostream>
#include "../PlayersOnTable.h"
#include "../Player.h"

void testPlayersOnTableCreation() {
    std::cout << "Running PlayersOnTable creation tests..." << std::endl;
    
    invido::PlayersOnTable players;
    
    assert(players.GetFirstOnMatch() == invido::NOT_VALID_INDEX);
    assert(players.GetFirstOnTrick() == invido::NOT_VALID_INDEX);
    assert(players.GetFirstOnGiocata() == invido::NOT_VALID_INDEX);
    
    std::cout << "PlayersOnTable creation tests PASSED" << std::endl;
}

void testPlayersOnTableSetFirst() {
    std::cout << "Running PlayersOnTable set first tests..." << std::endl;
    
    invido::PlayersOnTable players;
    players.SetFirstOnMatch(0);
    
    assert(players.GetFirstOnMatch() == 0);
    
    players.SetFirstOnTrick(1);
    assert(players.GetFirstOnTrick() == 1);
    
    players.SetFirstOnGiocata(2);
    assert(players.GetFirstOnGiocata() == 2);
    
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
    std::cout << "Running PlayersOnTable calc circle index tests..." << std::endl;
    
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

int main() {
    std::cout << "=== PlayersOnTable Unit Tests ===" << std::endl;
    
    testPlayersOnTableCreation();
    testPlayersOnTableSetFirst();
    testPlayersOnTableCreatePlayers();
    testPlayersOnTableCalcCircleIndex();
    
    std::cout << "=== All PlayersOnTable tests PASSED ===" << std::endl;
    return 0;
}
