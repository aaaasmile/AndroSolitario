#include <cassert>
#include <iostream>

#include "../Mazzo.h"

void testMazzoCreation() {
    std::cout << "Running Mazzo creation tests..." << std::endl;

    invido::Mazzo mazzo;
    mazzo.Init();

    std::cout << "Mazzo creation tests PASSED" << std::endl;
}

void testMazzoShuffle() {
    std::cout << "Running Mazzo shuffle tests..." << std::endl;

    invido::Mazzo mazzo;
    mazzo.Init();
    mazzo.SetRandomSeed(42);
    bool shuffled = mazzo.Shuffle();
    assert(shuffled);

    std::cout << "Mazzo shuffle tests PASSED" << std::endl;
}

void testMazzoPickCard() {
    std::cout << "Running Mazzo pick card tests..." << std::endl;

    invido::Mazzo mazzo;
    mazzo.Init();
    mazzo.SetRandomSeed(42);
    mazzo.Shuffle();

    invido::CardSpec card;
    bool hasCards = mazzo.PickNextCard(&card);
    assert(hasCards);
    assert(card.GetCardIndex() >= 0 && card.GetCardIndex() < 40);

    std::cout << "Mazzo pick card tests PASSED" << std::endl;
}

void testMazzoReset() {
    std::cout << "Running Mazzo reset tests..." << std::endl;

    invido::Mazzo mazzo;
    mazzo.Init();
    mazzo.SetRandomSeed(42);
    mazzo.Shuffle();

    invido::CardSpec card;
    mazzo.PickNextCard(&card);
    mazzo.PickNextCard(&card);

    mazzo.Reset();

    bool hasCards = mazzo.PickNextCard(&card);
    assert(hasCards);
    assert(card.GetCardIndex() >= 0 && card.GetCardIndex() < 40);

    std::cout << "Mazzo reset tests PASSED" << std::endl;
}

void testMazzo() {
    testMazzoCreation();
    testMazzoShuffle();
    testMazzoPickCard();
    testMazzoReset();
}
