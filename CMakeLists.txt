cmake_minimum_required(VERSION 3.27)
project(MaizeMix)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# Fetch SFML library
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(MaizeMix
        src/main.cpp
        src/MaizeMix/AudioClip.h
        src/MaizeMix/AudioEngine.cpp
        src/MaizeMix/AudioEngine.h
        src/MaizeMix/SoundReference.h
        src/MaizeMix/Music.h
        src/MaizeMix/Music.cpp
        src/MaizeMix/SoundReference.cpp
)

# Link sfml
target_link_libraries(MaizeMix
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        sfml-network
)