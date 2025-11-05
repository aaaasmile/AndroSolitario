cmake_minimum_required(VERSION 3.20)
project(hello)

# Don't mistake osx for unix
if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT RISCOS)
  set(UNIX_SYS ON)
else()
  set(UNIX_SYS OFF)
endif()

# Solitario program
set(SOURCES hello.c)

add_executable(${PROJECT_NAME} ${SOURCES} ${SOURCES2})

target_include_directories(${PROJECT_NAME} PRIVATE .)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wunused -g -O0>)

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows: " ${CMAKE_SYSTEM} " MySys: " ${CMAKE_HOST_WIN32})
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
        target_include_directories(${PROJECT_NAME} PRIVATE ../../app/jni/SDL/include ../../app/jni/SDL_image/include ../../app/jni/SDL_mixer/include ../../app/jni/SDL_ttf/include)
        target_compile_definitions(${PROJECT_NAME} PRIVATE TRACEINSERVICE _DEBUG WSL2_BUILD)
        ## use SDL from this compiled in this workspace
        set(SDL3_DIR "./build/SDL")
        #set(SDL3_ttf_DIR "../../build/SDL_ttf")
        add_subdirectory(../../app/jni/SDL ${CMAKE_BINARY_DIR}/SDL)
        add_subdirectory(../../app/jni/SDL_ttf ${CMAKE_BINARY_DIR}/SDL_ttf)

        # not needed to set: SDL3_image_DIR SDL3_mixer_DIR SDL3_ttf_DIR and find_package for SDL3_image, SDL3_mixer and SDL3_ttf 
        find_package(SDL3 REQUIRED)
        #find_package(SDL3_ttf REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            SDL3::SDL3
            SDL3_ttf::SDL3_ttf
        )
        message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory
              ${CMAKE_CURRENT_SOURCE_DIR}/fonts
              $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts
          COMMENT "Copying font to build directory"
        )
  else()
    message(STATUS "Target system is: " ${CMAKE_SYSTEM})
  endif()
endif()


