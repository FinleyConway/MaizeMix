option(BUILD_SHARED_LIBS "Build Maize Mix as a shared library" OFF)

# Fetch SFML library
include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_library(MaizeMix
        MaizeMix/AudioClip.h
        MaizeMix/AudioEngine.cpp
        MaizeMix/AudioEngine.h
        MaizeMix/SoundReference.h
        MaizeMix/SoundReference.cpp
        MaizeMix/Music.h
        MaizeMix/Music.cpp
        MaizeMix.h
)

# Link sfml
target_link_libraries(MaizeMix
        PRIVATE
        sfml-audio
)