# The main Photon executable. Add new source/header files to this list as the
# project grows (or group them into subdirectories with their own CMakeLists).
add_executable(photon
    main.cpp
    Color.cpp
    Object.cpp
    Camera.cpp
    Material.cpp
    BVH.cpp
)

target_link_libraries(photon PRIVATE glm::glm)

# Let sources include project headers as e.g. #include "core/vec3.hpp"
target_include_directories(photon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
