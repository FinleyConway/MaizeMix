cmake_minimum_required(VERSION 3.27)
project(AudioEngine)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch SFML library
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(AudioEngine
        main.cpp
        AudioClip.cpp
        AudioClip.h
        AudioEngine.cpp
        AudioEngine.h
)

# Link sfml
target_link_libraries(AudioEngine
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        sfml-network
)