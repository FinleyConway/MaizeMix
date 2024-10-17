# MaizeMix unit testing

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.7.1)
FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(test
        AudioEngine.test.cpp
        AudioManager.test.cpp
        SoundHandler.test.cpp
        StreamHandler.test.cpp
)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test PRIVATE
        MaizeMix
        Catch2::Catch2WithMain
)

# copy test clips to the build directory
add_custom_command(TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test/Clips
        ${CMAKE_BINARY_DIR}/test/Clips
)