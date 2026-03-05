#include <cassert>
#include <iostream>

#include "../Player.h"
#include "../PlayersOnTable.h"
#include "../InvidoCore.h"
#include "../Mazzo.h"

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
    invidoCore.Init(&playersOnTable, &mazzo);
    
    std::cout << "Match tests PASSED" << std::endl;
}

void testMatch() { testSimpleMatch(); }