cmake_minimum_required(VERSION 3.27)
project(MaizeMix)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MIX_BUILD_SHARED_LIBS "Build Maize Mix as a shared library" OFF)
set(MIX_BUILD_TESTBED ON)

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
        src/MaizeMix/Audio/Data/Clip.h
        src/MaizeMix/Audio/Music.cpp
        src/MaizeMix/Audio/Music.h
        src/MaizeMix/Audio/Data/SoundBuffer.cpp
        src/MaizeMix/Audio/Data/SoundBuffer.h
        src/MaizeMix/Audio/Data/SoundReference.cpp
        src/MaizeMix/Audio/Data/SoundReference.h
        src/MaizeMix/Engine/AudioEngine.cpp
        src/MaizeMix/Engine/AudioFinishCallback.h
        src/MaizeMix/Audio/AudioClip.cpp
        src/MaizeMix/Audio/AudioClip.h
        src/MaizeMix/Engine/AudioManager.cpp
        src/MaizeMix/Engine/AudioManager.h
)

target_include_directories(MaizeMix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (MIX_BUILD_TESTBED)
    add_subdirectory(testbed)
endif ()

target_link_libraries(MaizeMix PRIVATE sfml-audio)