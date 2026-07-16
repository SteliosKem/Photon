cmake_minimum_required(VERSION 3.15)
project(Photon
    VERSION 0.1.0
    DESCRIPTION "A custom ray tracer"
    LANGUAGES CXX)

# ---- Global settings ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to a Release build if the user didn't specify one.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- Dependencies ----
find_package(glm REQUIRED)

# ---- Targets ----
add_subdirectory(src)
