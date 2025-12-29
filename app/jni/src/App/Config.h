#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "Ver 3.0.6 20251229"
#define PACKAGE_URL "https://invido.it"
#define AUTHOR "igorrun"
#define VERSION_HOME "030005"

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
    #define HASTOUCH 0  // Remember that Emscripten run also on android with touch screen, but also mouse event are raised. Touch will be an event repetition.
    #define HASGOTLINK 0
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

#ifndef HASHELPMENU
    #ifdef PLATFORM_ANDROID
        #define HASHELPMENU 0  // Android usually uses system back button
    #else
        #define HASHELPMENU 1  // Enable on other platforms
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


#endif // CONFIG_H