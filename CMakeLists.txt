cmake_minimum_required(VERSION 3.24)
project(phys500_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PHYS500_FETCH_RAYLIB "Download raylib when it is not installed" ON)
set(PHYS500_RAYLIB_SOURCE_DIR "" CACHE PATH "Path to a local raylib source checkout")

if(PHYS500_RAYLIB_SOURCE_DIR)
    if(NOT EXISTS "${PHYS500_RAYLIB_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "PHYS500_RAYLIB_SOURCE_DIR must point to a raylib source directory containing CMakeLists.txt")
    endif()
    add_subdirectory("${PHYS500_RAYLIB_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/raylib-local" EXCLUDE_FROM_ALL)
else()
    find_package(raylib 5.0 QUIET)
endif()

if(NOT TARGET raylib AND PHYS500_FETCH_RAYLIB AND NOT PHYS500_RAYLIB_SOURCE_DIR)
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    message(STATUS "raylib was not found; downloading raylib 5.5 for this build")
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()

if(NOT TARGET raylib)
    message(FATAL_ERROR
        "raylib was not found. Install raylib, set -DPHYS500_RAYLIB_SOURCE_DIR=/path/to/raylib, or configure with -DPHYS500_FETCH_RAYLIB=ON.")
endif()

add_executable(phys500_homework
    src/main.cpp
)

target_link_libraries(phys500_homework PRIVATE raylib)

if(MSVC)
    target_compile_options(phys500_homework PRIVATE /W4)
else()
    target_compile_options(phys500_homework PRIVATE -Wall -Wextra -Wpedantic)
endif()
