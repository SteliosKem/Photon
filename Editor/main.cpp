#include <iostream>
/*#include <glm/glm.hpp>
#include "RayTracing/Material.h"
#include "RayTracing/Object.h"
#include "RayTracing/Camera.h"
#include "RayTracing/Texture.h"
#include "RayTracing/Utility.h"
#include "RayTracing/BVH.h"
#include "RayTracing/ConstantMedium.h"*/
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

int main() {
    /*ObjectList world;
    auto red   = make_shared<Metal>(Color(1, 1, 1), 0.1);
    auto white = make_shared<Diffuse>(Color(.73, .73, .73));
    auto green = make_shared<Metal>(Color(1, 1, 1), 0.1);
    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

    world.add(make_shared<Quad>(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), green));
    world.add(make_shared<Quad>(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), red));
    world.add(make_shared<Quad>(Vec3(113,554,127), Vec3(330,0,0), Vec3(0,0,305), light));
    world.add(make_shared<Quad>(Vec3(0,555,0), Vec3(555,0,0), Vec3(0,0,555), white));
    world.add(make_shared<Quad>(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white));
    world.add(make_shared<Quad>(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white));

    auto metal = make_shared<Metal>(Color(1, 1, 1), 0.1);
    auto glass = make_shared<Dielectric>(1.5);

    shared_ptr<Object> box1 = box(Vec3(0,0,0), Vec3(165,330,165), glass);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265,0,295));

    shared_ptr<Object> box2 = box(Vec3(0,0,0), Vec3(165,165,165), metal);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130,0,65));

    world.add(box1);
    world.add(box2);

    CameraAttributes atts;
   
    atts.aspect_ratio      = 1.0;
    atts.image_width       = 600;
    atts.samples_per_pixel = 100;
    atts.max_ray_depth         = 50;
    atts.background        = Color(0,0,0);

    atts.vfov     = 40;
    atts.pos      = Vec3(278, 278, 0);
    atts.direction   = Vec3(1, 0, 0.5);
    atts.vup      = Vec3(0,1,0);

    Camera cam(atts);
    cam.render(world);
    
    std::clog << "\rDone.                 \n";
    return 0;*/

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Photon Engine", nullptr, nullptr);

    VkInstance instance;

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Photon Editor";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Photon Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0); 

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    

    uint32_t extension_count = 0;
    const char** extensions = new const char*[3];

    extensions[0] = "VK_KHR_surface";
    extensions[1] = "VK_ext_metal_surface";
    extensions[2] = "VK_KHR_portability_enumeration";
    
    create_info.enabledExtensionCount = extension_count;
    create_info.ppEnabledExtensionNames = extensions;

    if(!vkCreateInstance(&create_info, 0, &instance)) { 
        std::cout << "Failed to create Vulkan instance.\n";
        return 1;
    }

    std::cout << "Vulkan instance created.\n";

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}