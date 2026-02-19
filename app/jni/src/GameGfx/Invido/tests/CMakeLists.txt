cmake_minimum_required(VERSION 3.20)

project(InvidoTests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SDL3 REQUIRED CONFIG PATHS "${CMAKE_SOURCE_DIR}/../../build/SDL" NO_DEFAULT_PATH)

set(TEST_INVIDO_DIR ${CMAKE_SOURCE_DIR}/..)
set(TEST_APP_DIR ${CMAKE_SOURCE_DIR}/../../../App)
set(TEST_SDL_DIR ${CMAKE_SOURCE_DIR}/../../../../SDL)

include_directories(
    ${TEST_INVIDO_DIR}
    ${TEST_APP_DIR}
    ${SDL3_INCLUDE_DIR}
)

set(TEST_SOURCES
    test_card.cpp
    test_mazzo.cpp
    test_playersontable.cpp
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${TEST_INVIDO_DIR}
    ${TEST_APP_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    SDL3::SDL3
)

enable_testing()
add_test(NAME InvidoTests COMMAND ${PROJECT_NAME})
