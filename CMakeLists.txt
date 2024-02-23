cmake_minimum_required(VERSION 3.27)
project(MaizeMix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MIX_BUILD_SHARED_LIBS "Build Maize Mix as a shared library" OFF)
set(MIX_BUILD_TESTBED OFF)

add_subdirectory(src/MaizeMix "MaizeMix")

if (MIX_BUILD_TESTBED)
    add_subdirectory(testbed)
endif ()