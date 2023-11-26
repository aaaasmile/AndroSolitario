cmake_minimum_required(VERSION 3.20)
project(solitario)

# Don't mistake osx for unix
if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT RISCOS)
  set(UNIX_SYS ON)
else()
  set(UNIX_SYS OFF)
endif()

# Solitario program
set(SOURCES GameGfx/CardRegionGfx.cpp
            GameGfx/CardStackGfx.cpp
            GameGfx/SolitarioGfx.cpp
            GameGfx/CurrentTime.cpp
            App/MusicManager.cpp
            App/AppGfx.cpp
            App/CardGfx.cpp
            App/DeckType.cpp
            App/GameSettings.cpp
            App/GfxUtil.cpp
            App/Languages.cpp
            App/Main.cpp
            App/Fading.c
            App/Credits.cpp
            App/ErrorInfo.cpp
            App/MenuMgr.cpp
            App/OptionsGfx.cpp
            App/TraceService.cpp
            App/HighScore.cpp
            App/WhereAmI.c
            CompGfx/LabelLinkGfx.cpp
            CompGfx/ButtonGfx.cpp
            CompGfx/CheckBoxGfx.cpp
            CompGfx/ComboGfx.cpp
            CompGfx/MesgBoxGfx.cpp
            CompGfx/LabelGfx.cpp)
if (${UNIX_SYS})
    set(SOURCES2 UnixSys/UnixsysTrace.cpp)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${SOURCES2})

target_include_directories(${PROJECT_NAME} PRIVATE App . GameGfx CompGfx)
target_compile_definitions(${PROJECT_NAME} PRIVATE DATA_PREFIX="data/")
target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wunused -g -O0>)

# ci vuole anche la piattaforma Platform: Linux-5.15.133.1-microsoft-standard-WSL2

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows: " ${CMAKE_SYSTEM} " MySys: " ${CMAKE_HOST_WIN32})
    if(CMAKE_HOST_WIN32)
        message(STATUS "OK MySys on Windows is a supported target") 
        # all following commands are additions, specific for the target MySys, to previous definitions
        target_sources(${PROJECT_NAME} PRIVATE mysys2/Util-mysys.cpp)
        target_compile_definitions(${PROJECT_NAME} PRIVATE main=SDL_main TRACEINSERVICE)
        target_include_directories(${PROJECT_NAME}  PRIVATE C:/msys64/ucrt64/include/SDL2)
        target_link_libraries(${PROJECT_NAME} mingw32 -mwindows SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)
    endif()
else()
    message(STATUS "Target system is: " ${CMAKE_SYSTEM})
    target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE)
    target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image SDL2_mixer SDL2_ttf)
endif()

target_link_libraries(${PROJECT_NAME} libini)

