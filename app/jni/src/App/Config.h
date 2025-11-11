#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "Ver 3.0.2 20251111"
#define PACKAGE_URL "https://invido.it"
#define AUTHOR "igorrun"

// Platform detection
#if defined(__ANDROID__)
    #define PLATFORM_ANDROID 1
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#elif defined(__APPLE__)
    #define PLATFORM_APPLE 1
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

#endif // CONFIG_H