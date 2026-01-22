#ifndef _WINTYPEGLOBAL__H_
#define _WINTYPEGLOBAL__H_

#include <stdarg.h>

#include <string>
#include <vector>

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef STRING
typedef std::string STRING;
#endif

#ifndef LPCSTR
typedef const char *LPCSTR, *PCSTR;
#endif

#ifndef TRACE
#ifdef TRACEINDEBUGGER
inline void TRACE(const char* fmt, ...) {
    char myBuff[512];
    va_list args;
    va_start(args, fmt);
    int result = vsprintf(myBuff, fmt, args);
    ::OutputDebugString(myBuff);
}
inline void TRACE_DEBUG(const char* fmt, ...) {}
#else
#ifdef TRACEINSERVICE
extern void TraceInServiceINFO(char* myBuff);
extern void TraceInServiceDEBUG(char* myBuff);
inline void TRACE(const char* fmt, ...) {
    char myBuff[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(myBuff, fmt, args);
    TraceInServiceINFO(myBuff);
}
#ifdef _DEBUG
inline void TRACE_DEBUG(const char* fmt, ...) {
    char myBuff[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(myBuff, fmt, args);
    TraceInServiceDEBUG(myBuff);
}
#else
inline void TRACE_DEBUG(const char* fmt, ...) {}
#endif
#else
#ifdef TRACEINSTD
#include <stdio.h>
inline void TRACE(const char* fmt, ...) {
    char myBuff[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(myBuff, fmt, args);
    sprintf(stdout, myBuff);
}
#else
inline void TRACE(const char* fmt, ...) {}
#endif
inline void TRACE_DEBUG(const char* fmt, ...) {}
#endif
#endif
#endif

#ifndef VCT_STRING
typedef std::vector<std::string> VCT_STRING;
#endif


#if USE_EMPTY_DATA_PREFIX
    #define DATA_PREFIX
#endif

#endif