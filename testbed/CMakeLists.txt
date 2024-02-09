project(testbed)

add_executable(testbed
    main.cpp
)

target_include_directories(testbed PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(testbed PUBLIC MaizeMix sfml-window sfml-audio)