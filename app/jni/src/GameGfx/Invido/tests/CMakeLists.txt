cmake_minimum_required(VERSION 3.20)

project(InvidoTests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# SDL finding strategy: 
# If SDL3 target doesn't exist (standalone build), find it.
if(NOT TARGET SDL3::SDL3)
    find_package(SDL3 REQUIRED CONFIG PATHS "${CMAKE_SOURCE_DIR}/../../build/SDL" NO_DEFAULT_PATH)
endif()

# Correct paths relative to this CMakeLists.txt
set(TEST_INVIDO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TEST_APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../App)

# Include directories for headers
include_directories(
    ${TEST_INVIDO_DIR}
    ${TEST_APP_DIR}
    ${SDL3_INCLUDE_DIR}
)

# Implementation files needed for linking
set(TEST_IMPL_SOURCES
    ${TEST_INVIDO_DIR}/CardSpec.cpp
    ${TEST_INVIDO_DIR}/Mazzo.cpp
    ${TEST_INVIDO_DIR}/PlayersOnTable.cpp
    ${TEST_INVIDO_DIR}/Player.cpp
    ${TEST_INVIDO_DIR}/PlayerStatistic.cpp
    ${TEST_INVIDO_DIR}/AlgPlayer.cpp
)

set(TEST_SOURCES
    main_test.cpp
    test_card.cpp
    test_mazzo.cpp
    test_playersontable.cpp
    ${TEST_IMPL_SOURCES}
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
