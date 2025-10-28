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

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows: " ${CMAKE_SYSTEM} " MySys: " ${CMAKE_HOST_WIN32})
    if(CMAKE_HOST_WIN32)
        message(STATUS "OK MySys on Windows is a supported target") 
        # all following commands are additions, specific for the target MySys, to previous definitions
        target_sources(${PROJECT_NAME} PRIVATE mysys2/Util-mysys.cpp)
        target_compile_definitions(${PROJECT_NAME} PRIVATE main=SDL_main TRACEINSERVICE)
        target_include_directories(${PROJECT_NAME}  PRIVATE C:/msys64/ucrt64/include/SDL3)
        target_link_libraries(${PROJECT_NAME} mingw32 -mwindows SDL3main SDL3 SDL3_image SDL3_mixer SDL3_ttf)
    endif()
else()
  if(CMAKE_SYSTEM MATCHES ".*WSL2.*" OR CMAKE_SYSTEM MATCHES ".*microsoft.*")
        message(STATUS "Target system is WSL2: " ${CMAKE_SYSTEM})
        # In your CMakeLists.txt
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        set(CMAKE_CXX_EXTENSIONS OFF)
        message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
        message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")
        message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
        target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
        target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11)
        target_include_directories(${PROJECT_NAME} PRIVATE ../SDL/include ../SDL_image/include ../SDL_mixer/include ../SDL_ttf/include)
        target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE _DEBUG WSL2_BUILD)
        target_link_libraries(${PROJECT_NAME} SDL3 SDL3_image SDL3_mixer SDL3_ttf)
        # Add any WSL2-specific configurations here
  else()
    message(STATUS "Target system is: " ${CMAKE_SYSTEM})
    target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE _DEBUG)
    target_link_libraries(${PROJECT_NAME} SDL3 SDL3_image SDL3_mixer SDL3_ttf)
  endif()
endif()


