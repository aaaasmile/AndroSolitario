#include <cassert>
#include <iostream>

#include "../Player.h"
#include "../PlayersOnTable.h"

void testRandomMatch(){
    invido::PlayersOnTable playersOnTable;
    invido::Player player1;
    invido::Player player2;

    player1.Init(NULL, invido::PT_MACHINE, "Guido", 0);
    player2.Init(NULL, invido::PT_MACHINE, "Rensu", 1);
    playersOnTable.AddPlayer(player1);
    playersOnTable.AddPlayer(player2);
}

void testMatch(){
    testRandomMatch();
}