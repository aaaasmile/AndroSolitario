#include <cassert>
#include <iostream>
#include "../CardSpec.h"
#include "../Mazzo.h"

void testCardSpec() {
    std::cout << "Running CardSpec tests..." << std::endl;
    
    invido::CardSpec card;
    card.SetCardIndex(0);
    assert(card.GetCardIndex() == 0);
    assert(card.GetSuit() == invido::BASTONI);
    
    card.SetCardIndex(1);
    assert(card.GetCardIndex() == 1);
    assert(card.GetSuit() == invido::BASTONI);
    
    card.SetCardIndex(10);
    assert(card.GetSuit() == invido::COPPE);
    
    card.SetCardIndex(20);
    assert(card.GetSuit() == invido::DENARI);
    
    card.SetCardIndex(30);
    assert(card.GetSuit() == invido::SPADE);
    
    std::cout << "CardSpec tests PASSED" << std::endl;
}

void testMazzo() {
    std::cout << "Running Mazzo tests..." << std::endl;
    
    invido::Mazzo mazzo;
    mazzo.Create();
    mazzo.SetRandomSeed(42);
    mazzo.Shuffle();
    
    invido::CardSpec card;
    bool hasCards = mazzo.PickNextCard(&card);
    assert(hasCards);
    assert(card.GetCardIndex() >= 0 && card.GetCardIndex() < 40);
    
    mazzo.Reset();
    hasCards = mazzo.PickNextCard(&card);
    assert(hasCards);
    
    for (int i = 0; i < 39; i++) {
        mazzo.PickNextCard(&card);
    }
    hasCards = mazzo.PickNextCard(&card);
    assert(!hasCards);
    
    std::cout << "Mazzo tests PASSED" << std::endl;
}

int main() {
    std::cout << "=== Invido Unit Tests ===" << std::endl;
    
    testCardSpec();
    testMazzo();
    
    std::cout << "=== All tests PASSED ===" << std::endl;
    return 0;
}
