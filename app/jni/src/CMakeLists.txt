cmake_minimum_required(VERSION 3.20)
project(solitario VERSION 3.0.10 LANGUAGES C CXX)

# Don't mistake osx for unix
if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT RISCOS)
  set(UNIX_SYS ON)
else()
  set(UNIX_SYS OFF)
endif()

# Solitario program
set(SOURCES App/MusicManager.cpp
            App/AppGfx.cpp
            App/CardGfx.cpp
            App/DeckType.cpp
            App/GameSettings.cpp
            App/GfxUtil.cpp
            App/Languages.cpp
            App/Main.cpp
            App/Fading.cpp
            App/Credits.cpp
            App/ErrorInfo.cpp
            App/MenuMgr.cpp
            App/OptionsGfx.cpp
            App/TraceService.cpp
            App/HighScore.cpp
            App/CurrentTime.cpp
            App/GameSelector.cpp
            CompGfx/LabelLinkGfx.cpp
            CompGfx/ButtonGfx.cpp
            CompGfx/CheckBoxGfx.cpp
            CompGfx/ComboGfx.cpp
            CompGfx/MesgBoxGfx.cpp
            CompGfx/LabelGfx.cpp
            CompGfx/TextInputGfx.cpp
            CompGfx/KeyboardGfx.cpp
            GameGfx/Solitario/CardRegionGfx.cpp
            GameGfx/Solitario/CardStackGfx.cpp
            GameGfx/Solitario/SolitarioGfx.cpp)
if (${UNIX_SYS})
    set(SOURCES2 UnixSys/UnixsysTrace.cpp)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${SOURCES2})

target_include_directories(${PROJECT_NAME} PRIVATE App . GameGfx CompGfx)
target_compile_definitions(${PROJECT_NAME} PRIVATE DATA_PREFIX="data/")

if(CMAKE_SYSTEM MATCHES Windows)
    message(STATUS "Target system is Windows: " ${CMAKE_SYSTEM} " MySys: " ${CMAKE_HOST_WIN32})
    # Configure the .rc file for windows
    set(ICON_PATH "${CMAKE_SOURCE_DIR}/assets/icons/tressette.ico")
    set(PROJECT_COMPANY "IgorRun for Invido.it")
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows_icon.rc.in
        ${CMAKE_BINARY_DIR}/windows_icon.rc
        @ONLY
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # General C/C++ optimizations
        message(STATUS "=> Building in RELEASE mode")
        add_compile_options(
            -O3           # Maximum optimization
            -DNDEBUG      # Disable debug code
            -flto         # Link-time optimization
        )
    else()
      target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wunused -g -O0>)
    endif()
    if(CMAKE_HOST_WIN32)
        message(STATUS "*** > OK MySys on Windows is a supported target, source dir is ${CMAKE_SOURCE_DIR}") 
        # all following commands are additions, specific for the target MySys, to previous definitions
        target_sources(${PROJECT_NAME} PRIVATE mysys2/Util-mysys.cpp)
        target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/windows_icon.rc)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
          target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE)
          target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
        else()
          target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG TRACEINSERVICE)
        endif()
        target_include_directories(${PROJECT_NAME} PRIVATE ../SDL/include ../SDL_image/include ../SDL_mixer/include ../SDL_ttf/include)
        set(SDL3_DIR "../../../build/SDL")
        # not needed to set: SDL3_image_DIR SDL3_mixer_DIR SDL3_ttf_DIR and find_package for SDL3_image, SDL3_mixer and SDL3_ttf 
        find_package(SDL3 REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            SDL3::SDL3
            SDL3_image::SDL3_image
            SDL3_mixer::SDL3_mixer  
            SDL3_ttf::SDL3_ttf
        )
        set_target_properties(solitario PROPERTIES
            OUTPUT_NAME "solitario"
            DEBUG_POSTFIX ".debug"
            RELEASE_POSTFIX ""
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        # copy asset in build/bin
        message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory
              ${CMAKE_CURRENT_SOURCE_DIR}/../../src/main/assets/
              $<TARGET_FILE_DIR:${PROJECT_NAME}>/data/
          COMMENT "Copying assets directory to build directory"
        )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory
              ${CMAKE_CURRENT_SOURCE_DIR}/../../../../MySys2_dlls/
              $<TARGET_FILE_DIR:${PROJECT_NAME}>/
          COMMENT "Copying dlls to build directory"
        )
    endif()
else()
  if(CMAKE_SYSTEM MATCHES ".*WSL2.*" OR CMAKE_SYSTEM MATCHES ".*microsoft.*")
        message(STATUS "Target system is WSL2: " ${CMAKE_SYSTEM})
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        set(CMAKE_CXX_EXTENSIONS OFF)
        message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
        message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")
        message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
        target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
        target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11)
        target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wunused -g -O0>)
        target_include_directories(${PROJECT_NAME} PRIVATE ../SDL/include ../SDL_image/include ../SDL_mixer/include ../SDL_ttf/include)
        target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE _DEBUG WSL2_BUILD)
        ## use SDL from this compiled project
        set(SDL3_DIR "../../../build/SDL")
        # not needed to set: SDL3_image_DIR SDL3_mixer_DIR SDL3_ttf_DIR and find_package for SDL3_image, SDL3_mixer and SDL3_ttf 
        find_package(SDL3 REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            SDL3::SDL3
            SDL3_image::SDL3_image
            SDL3_mixer::SDL3_mixer  
            SDL3_ttf::SDL3_ttf
        )
        # Add any WSL2-specific configurations here
        # Set a new output name and different postfixes for Debug/Release builds
        set_target_properties(solitario PROPERTIES
            OUTPUT_NAME "solitario"
            DEBUG_POSTFIX ".debug"
            RELEASE_POSTFIX ""
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        # copy asset in build/bin
        message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory
              ${CMAKE_CURRENT_SOURCE_DIR}/../../src/main/assets/
              $<TARGET_FILE_DIR:${PROJECT_NAME}>/data/
          COMMENT "Copying assets directory to build directory"
        )
  else()
    if(CMAKE_SYSTEM MATCHES ".*Emscripten.*")
      message(STATUS "** SRC Target system is: ${CMAKE_SYSTEM} build SDL is: ${CMAKE_BINARY_DIR}/SDL")
      target_include_directories(${PROJECT_NAME} PRIVATE ../SDL/include ../SDL_image/include ../SDL_mixer/include ../SDL_ttf/include)
      set(SDL3_DIR "${CMAKE_BINARY_DIR}/SDL")
      # not needed to set: SDL3_image_DIR SDL3_mixer_DIR SDL3_ttf_DIR and find_package for SDL3_image, SDL3_mixer and SDL3_ttf 
      find_package(SDL3 REQUIRED)
      target_link_libraries(${PROJECT_NAME}
          SDL3::SDL3
          SDL3_image::SDL3_image
          SDL3_mixer::SDL3_mixer  
          SDL3_ttf::SDL3_ttf
      )
      # NOTE: I am using MyRelease because in Release the SDL_ttf doesn't show emoj fonts
      if(CMAKE_BUILD_TYPE STREQUAL "MyRelease")
        message(STATUS "** Building Myrelease for: " ${CMAKE_SYSTEM})
        target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE)
        add_compile_options(
            -O3
            -DNDEBUG      
            -flto         
        )
        # note the SHELL syntax for the -s parameter
        target_link_options(${PROJECT_NAME} PRIVATE
            -O3
            "SHELL:-s ASSERTIONS=0"
            "SHELL:-s GL_ASSERTIONS=0"
            "SHELL:-s GL_DEBUG=0"
            "SHELL:-s INITIAL_MEMORY=67108864"  
            "SHELL:-s MAXIMUM_MEMORY=2147483648"
            "SHELL:-s SAFE_HEAP=0"
            "SHELL:-s STACK_OVERFLOW_CHECK=0"
            "SHELL:-s ALLOW_MEMORY_GROWTH=1"
            "SHELL:--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/../../src/main/assets@/data"
        )
      else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE _DEBUG)
        target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wunused -g -O0>)
        set_target_properties(${PROJECT_NAME} PROPERTIES
          LINK_FLAGS "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/../../src/main/assets@/data -s ALLOW_MEMORY_GROWTH=1"
        )
      endif()
      # Add any Emscripten-specific configurations here
      # Set a new output name and different postfixes for Debug/Release builds
      set_target_properties(solitario PROPERTIES
          OUTPUT_NAME "${PROJECT_NAME}_${PROJECT_VERSION}"
          DEBUG_POSTFIX ".debug"
          RELEASE_POSTFIX ""
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
      )
      add_compile_options(-s BUILD_AS=PROJECT_${PROJECT_VERSION})
      # copy asset in build/bin
      message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/emscripten/
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying stuff for web"
      )
    else()
      # Note on Android: it is built using gradle and mk files, so no cmake section here
      message(STATUS "=> TODO add cmake support please this Target: " ${CMAKE_SYSTEM})
    endif()  
  endif()
endif()


