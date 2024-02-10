project(testbed)

include(FetchContent)

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.12.2
)

FetchContent_MakeAvailable(entt)

add_executable(testbed
    main.cpp
        test_AudioSystem.cpp
        test_AudioSystem.h
        test_Components.h
)

target_include_directories(testbed PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(testbed PUBLIC MaizeMix sfml-window sfml-audio EnTT::EnTT)