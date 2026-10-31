cmake_minimum_required(VERSION 3.20)
project(sdl3_harness LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app/jni/SDL
                 ${CMAKE_CURRENT_BINARY_DIR}/SDL)

add_executable(sdl3_harness main.cpp)
target_link_libraries(sdl3_harness PRIVATE SDL3::SDL3)
