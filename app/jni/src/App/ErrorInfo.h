#ifndef __ERROR_INFO_INC
#define __ERROR_INFO_INC

#include <string>

typedef struct ErrInApp {
    std::string ErrorText;
} ErrInApp, *LPErrInApp;

namespace ERR_UTIL {
    LPErrInApp ErrorCreate(const char* errtext);
    LPErrInApp ErrorCreate(const char* format, const char* errtext);
    LPErrInApp ErrorCreate(const char* format, const int val);
    LPErrInApp ErrorCreate(const char* format, const char* errtext,
                           const char* errtext2);
};

#endif