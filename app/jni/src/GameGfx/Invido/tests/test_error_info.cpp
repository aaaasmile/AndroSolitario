#include <array>
#include <cassert>
#include <iostream>

#include "../../../App/ErrorInfo.h"

void testErrorInfo() {
    std::cout << "Running ErrorInfo ring-buffer tests..." << std::endl;

    std::array<LPErrInApp, 10> errorSlots;
    for (int index = 0; index < static_cast<int>(errorSlots.size()); ++index) {
        errorSlots[index] = ERR_UTIL::ErrorCreate("error %d", index);
        assert(errorSlots[index] != NULL);
        assert(errorSlots[index]->ErrorText == "error " + std::to_string(index));
        for (int previousIndex = 0; previousIndex < index; ++previousIndex) {
            assert(errorSlots[index] != errorSlots[previousIndex]);
        }
    }

    LPErrInApp reusedSlot = ERR_UTIL::ErrorCreate("error %d", 10);
    assert(reusedSlot == errorSlots[0]);
    assert(reusedSlot->ErrorText == "error 10");
    assert(errorSlots[1]->ErrorText == "error 1");

    std::cout << "ErrorInfo ring-buffer tests PASSED" << std::endl;
}
