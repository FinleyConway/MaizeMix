cmake_minimum_required(VERSION 3.27)
project(MaizeMix)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MIX_BUILD_SHARED_LIBS "Build Maize Mix as a shared library" OFF)
option(MIX_BUILD_TEST "Build test project" OFF)

# import sfml
FetchContent_Declare(sfml GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.1)
FetchContent_MakeAvailable(sfml)

# build it as a static or shared library
if (MIX_BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

add_library(MaizeMix ${LIB_TYPE}
        src/MaizeMix/Helper/Clip.h
        src/MaizeMix/Helper/SoundData.h
        src/MaizeMix/Helper/AudioManager.cpp
        src/MaizeMix/Helper/AudioManager.h

        src/MaizeMix/Sounds/SoundHandler.cpp
        src/MaizeMix/Sounds/SoundHandler.h
        src/MaizeMix/Sounds/SoundBuffer.cpp
        src/MaizeMix/Sounds/SoundBuffer.h

        src/MaizeMix/Streaming/StreamHandler.cpp
        src/MaizeMix/Streaming/SoundReference.h
        src/MaizeMix/Streaming/SoundReference.cpp
        src/MaizeMix/Streaming/StreamHandler.h
        src/MaizeMix/Streaming/Music.cpp
        src/MaizeMix/Streaming/Music.h

        src/MaizeMix/AudioClip.cpp
        src/MaizeMix/AudioClip.h
        src/MaizeMix/AudioEngine.cpp
        src/MaizeMix/AudioEngine.h
        src/MaizeMix.h
)

target_include_directories(MaizeMix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(MaizeMix PUBLIC sfml-audio)

# add test directory if tests are enabled
if (NOT MIX_BUILD_SHARED_LIBS AND MIX_BUILD_TEST)
    add_subdirectory(test)
endif()
