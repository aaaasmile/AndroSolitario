#ifndef CONFIG_H
#define CONFIG_H

#include <stdarg.h>

#include <cstdio>
#include <string>
#include <vector>

#define VERSION "Ver 3.0.13 20260201-02"
#define PACKAGE_URL "https://invido.it"
#define AUTHOR "igorRun for Invido"

#define VERSION_HOME \
    "030007"  // this is used to reset settings and score in windows

// Platform detection
#if defined(__ANDROID__) || defined(ANDROID)
#define PLATFORM_ANDROID 1
#elif defined(EMSCRIPTEN)
#define PLATFORM_EMS 1
#elif defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define PLATFORM_LINUX 1
#elif defined(__APPLE__)
#define PLATFORM_APPLE 1
#endif

#if PLATFORM_EMS
#define HASWINICON 0
#define HASQUITMENU 0
#define HASHELPMENU 0
#define HASTOUCH \
    0  // Remember that Emscripten run also on android with touch screen, but
       // also mouse event are raised. Touch will be an event repetition.
#define HASGOTLINK 1
#endif

#ifndef HASWINICON
#define HASWINICON 1
#endif

#ifndef HASQUITMENU
#ifdef PLATFORM_ANDROID
#define HASQUITMENU 0  // Android usually uses system back button
#else
#define HASQUITMENU 1  // Enable on other platforms
#endif
#endif

#ifndef HASFULLSCREEN
#ifdef PLATFORM_ANDROID
#define HASFULLSCREEN 1  // Android is usually fullscreen by default
#else
#define HASFULLSCREEN 0
#endif
#endif

#ifndef HASTOUCH
#ifdef PLATFORM_ANDROID
#define HASTOUCH 1  // Android is touch
#else
#define HASTOUCH 0
#endif
#endif

#ifndef HASMOUSE
#ifdef PLATFORM_ANDROID
#define HASMOUSE 0  // Android no mouse support
#else
#define HASMOUSE 1
#endif
#endif

#ifndef HASGOTLINK
#define HASGOTLINK 1
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

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef STRING
typedef std::string STRING;
#endif

#ifndef LPCSTR
typedef const char *LPCSTR, *PCSTR;
#endif

#ifndef VCT_STRING
typedef std::vector<std::string> VCT_STRING;
#endif

#if USE_EMPTY_DATA_PREFIX
#define DATA_PREFIX
#endif

#endif  // CONFIG_H