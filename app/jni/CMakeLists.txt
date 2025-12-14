cmake_minimum_required(VERSION 3.6)

project(solitario)

# armeabi-v7a requires cpufeatures library
# include(AndroidNdkModules)
# android_ndk_import_module_cpufeatures()

message(STATUS "** => Target system is : " ${CMAKE_SYSTEM})

# SDL sources are in a subfolder named "SDL"
add_subdirectory(SDL)

# Compilation of companion libraries
add_subdirectory(SDL_image)
add_subdirectory(SDL_mixer)

if(CMAKE_SYSTEM MATCHES ".*Emscripten.*")
    message(STATUS "=> Target Emscripten for WASM")
    #nothing to do here, sub project will overwrite it
endif()

add_subdirectory(SDL_ttf)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(src)

