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

# Don't build GLFW examples/tests/docs
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# ---- Dependencies ----
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

# ---- Targets ----
add_subdirectory("ThirdParty/glfw")
add_subdirectory(Engine)
add_subdirectory(Editor)