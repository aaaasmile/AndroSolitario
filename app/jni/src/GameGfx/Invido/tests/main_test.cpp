#include <iostream>

void testCardSpec();
void testMazzo();
void testPlayersOnTable();
void testMatch();

int main(int argc, char* argv[]) {
    std::cout << "=== Invido Unit Tests Runner ===" << std::endl;

    try {
        testCardSpec();
        testMazzo();
        testPlayersOnTable();
        testMatch();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }

    std::cout << "=== All tests PASSED ===" << std::endl;
    return 0;
}
