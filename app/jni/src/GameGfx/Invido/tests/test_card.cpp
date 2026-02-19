#include <cassert>
#include <iostream>

#include "../CardSpec.h"

void testCardSpec() {
    std::cout << "Running CardSpec tests..." << std::endl;

    invido::CardSpec card;
    card.SetCardIndex(0);
    assert(card.GetCardIndex() == 0);
    assert(card.GetSuit() == eSUIT::BASTONI);

    card.SetCardIndex(1);
    assert(card.GetCardIndex() == 1);
    assert(card.GetSuit() == eSUIT::BASTONI);

    card.SetCardIndex(10);
    assert(card.GetSuit() == eSUIT::COPPE);

    card.SetCardIndex(20);
    assert(card.GetSuit() == eSUIT::DENARI);

    card.SetCardIndex(30);
    assert(card.GetSuit() == eSUIT::SPADE);

    std::cout << "CardSpec tests PASSED" << std::endl;
}
