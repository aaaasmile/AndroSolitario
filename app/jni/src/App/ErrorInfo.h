#ifndef __ERROR_INFO_INC
#define __ERROR_INFO_INC

#include <string>

typedef struct ErrInApp {
    std::string ErrorText;
} ErrInApp, *LPErrInApp;

namespace ERR_UTIL {
    // Returns one of 10 rotating static slots; this is not thread-safe.
    // Do not delete it: its message can be overwritten after 10 more calls.
    LPErrInApp ErrorCreate(const char* errtext);
    LPErrInApp ErrorCreate(const char* format, const char* errtext);
    LPErrInApp ErrorCreate(const char* format, const int val);
    LPErrInApp ErrorCreate(const char* format, const char* errtext,
                           const char* errtext2);
};

#endif