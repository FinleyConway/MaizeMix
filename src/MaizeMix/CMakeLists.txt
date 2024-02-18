option(BUILD_SHARED_LIBS "Build Maize Mix as a shared library" OFF)

# Fetch SFML library
include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# Define the library type based on the BUILD_SHARED_LIBS option
if(BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

add_library(MaizeMix ${LIB_TYPE}
        Audio/AudioClip.h
        Engine/AudioEngine.cpp
        Engine/AudioEngine.h
        Audio/SoundReference.h
        Audio/SoundReference.cpp
        Audio/Music.h
        Audio/Music.cpp
        MaizeMix.h
        Engine/AudioFinishCallback.h
)

# Link sfml
target_link_libraries(MaizeMix
        PRIVATE
        sfml-audio
)