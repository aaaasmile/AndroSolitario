#include "ErrorInfo.h"

#include <cstddef>

namespace {

const std::size_t kErrorSlotCount = 10;
ErrInApp g_errorSlots[kErrorSlotCount];
std::size_t g_nextErrorSlot = 0;

LPErrInApp GetNextErrorSlot() {
    ErrInApp* error = &g_errorSlots[g_nextErrorSlot];
    g_nextErrorSlot = (g_nextErrorSlot + 1) % kErrorSlotCount;
    error->ErrorText.clear();
    return error;
}

}  // namespace

LPErrInApp ERR_UTIL::ErrorCreate(const char* errtext) {
    LPErrInApp res = GetNextErrorSlot();
    res->ErrorText = errtext;
    return res;
}

LPErrInApp ERR_UTIL::ErrorCreate(const char* format, const char* errtext) {
    LPErrInApp res = GetNextErrorSlot();
    char buff[1024];
    snprintf(buff, sizeof(buff), format, errtext);
    res->ErrorText = buff;
    return res;
}

LPErrInApp ERR_UTIL::ErrorCreate(const char* format, const char* errtext,
                                 const char* errtext2) {
    LPErrInApp res = GetNextErrorSlot();
    char buff[1024];
    snprintf(buff, sizeof(buff), format, errtext, errtext2);
    res->ErrorText = buff;
    return res;
}

LPErrInApp ERR_UTIL::ErrorCreate(const char* format, const int val) {
    LPErrInApp res = GetNextErrorSlot();
    char buff[1024];
    snprintf(buff, sizeof(buff), format, val);
    res->ErrorText = buff;
    return res;
}
