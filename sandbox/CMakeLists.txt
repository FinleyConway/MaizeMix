# MaizeMix sandbox testing

FetchContent_Declare(flecs GIT_REPOSITORY https://github.com/SanderMertens/flecs.git GIT_TAG v4.0.2)
FetchContent_MakeAvailable(flecs)

add_executable(sandbox
        main.cpp
)

target_include_directories(sandbox PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(sandbox PRIVATE
        MaizeMix
        flecs::flecs_static
        sfml-graphics
)

# copy test clips to the build directory
add_custom_command(TARGET sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/sandbox/Clips
        ${CMAKE_BINARY_DIR}/sandbox/Clips
)