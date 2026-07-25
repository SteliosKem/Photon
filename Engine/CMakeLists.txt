# The main Photon Engine library. Add new source/header files to this list as the
# project grows (or group them into subdirectories with their own CMakeLists).
add_library(PhotonEngine STATIC
    Core/Window.cpp
    Core/Application.cpp

    RayTracing/Color.cpp
    RayTracing/Object.cpp
    RayTracing/Camera.cpp
    RayTracing/Material.cpp
    RayTracing/BVH.cpp
    RayTracing/Texture.cpp
    RayTracing/ConstantMedium.cpp
)

target_link_libraries(PhotonEngine
PUBLIC
    glm::glm
    Vulkan::Vulkan
    glfw
)

# Let sources include project headers as e.g. #include "core/vec3.hpp"
target_include_directories(PhotonEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
