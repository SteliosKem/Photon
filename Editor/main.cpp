#include <iostream>
#include <vulkan/vulkan.h>
#include "Core/Application.h"

using namespace Photon;

int main() {
    ApplicationInfo app_info{};
    app_info.name = "Photon Engine";
    app_info.version = Version{1, 0 ,0};

    WindowInfo main_win_info{};
    main_win_info.title = "Photon Engine";
    app_info.main_window_info = main_win_info;

    Application app(app_info);

    VkInstance instance;

    VkApplicationInfo vk_app_info{};
    vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_app_info.pApplicationName = "Photon Editor";
    vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_app_info.pEngineName = "Photon Engine";
    vk_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0); 

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &vk_app_info;
    

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

    if(app.run() != ErrorCode::OK) return 1;

    return 0;
}