cmake_minimum_required(VERSION 3.15)
project(Photon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glm REQUIRED)

add_executable(photon main.cpp)
target_link_libraries(photon PRIVATE glm::glm)
