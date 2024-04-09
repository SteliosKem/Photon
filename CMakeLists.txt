cmake_minimum_required(VERSION 3.0.0)
project(Zeus VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(Zeus src/main.cpp
    src/glad.c
    src/imgui_spectrum.cpp
)

target_include_directories(Zeus PRIVATE include)

set(IMGUI_PATH  "/home/stylianos/Downloads/imgui/")

# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/*.cpp) 
add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends/)

target_link_libraries(Zeus PUBLIC ImGui glfw)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
