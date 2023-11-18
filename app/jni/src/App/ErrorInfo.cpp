#include "ErrorInfo.h"

LPErrInApp ERR_UTIL::ErrorCreate(const char* errtext) {
    LPErrInApp res = new ErrInApp();
    res->ErrorText = errtext;
    return res;
}

LPErrInApp ERR_UTIL::ErrorCreate(const char* format, const char* errtext) {
    LPErrInApp res = new ErrInApp();
    char buff[1024];
    snprintf(buff, sizeof(buff), format, errtext);
    res->ErrorText = buff;
    return res;
}

LPErrInApp ERR_UTIL::ErrorCreate(const char* format, const char* errtext,
                                 const char* errtext2) {
    LPErrInApp res = new ErrInApp();
    char buff[1024];
    snprintf(buff, sizeof(buff), format, errtext, errtext2);
    res->ErrorText = buff;
    return res;
}

LPErrInApp ERR_UTIL::ErrorCreate(const char* format, const int val) {
    LPErrInApp res = new ErrInApp();
    char buff[1024];
    snprintf(buff, sizeof(buff), format, val);
    res->ErrorText = buff;
    return res;
}
