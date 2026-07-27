#include "Application.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

const Filepath VERTEX_PATH = "Shaders/basicVertex.vert.spv";
const Filepath FRAGMENT_PATH = "Shaders/basicFragment.frag.spv";

namespace Photon {
    Application::Application(const ApplicationInfo& app_info)
        : m_app_info{ app_info }, m_window(app_info.main_window_info)
        , m_pipeline(VERTEX_PATH, FRAGMENT_PATH) {
        init();
    }

    Application::~Application() {
        shutdown();
    }

    bool Application::init() {
        glfwInit();
        m_window.init_window();

        if (!create_vulkan_instance()) {
            std::cout << "Failed to initialize Vulkan instance.\n";
            return false;
        }

        std::cout << "Initialized Vulkan instance.\n";

        return true;
    }

    void Application::shutdown() {
        if (m_vulkan_instance) {
            vkDestroyInstance(m_vulkan_instance, nullptr);
        }
        volk_finalize();

        m_window.destroy_window();
        glfwTerminate();
    }

    bool Application::init_vulkan() {
        if (!create_vulkan_instance()) {
            // Temp logging
            std::cout << "Failed to create Vulkan instance.\n";
            return false;
        }
    }

    bool Application::create_vulkan_instance() {
        if (!init_volk()) {
            std::cout << "Failed to initialize Volk.\n";
            return false;
        }

        VkApplicationInfo app_info{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = m_app_info.name.c_str(),
            .apiVersion = VULKAN_VERSION
        };

        // Find required extensions for the platform
        u32 extension_count{};
        const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);

        // Add debug extension first
        vector<const char*> requested_extensions{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
        for (u32 i = 0; i < extension_count; i++) {
            requested_extensions.push_back(extensions[i]);
        }

        std::vector<const char*> requested_layers{ "VK_LAYER_KHRONOS_validation" };

        VkDebugUtilsMessengerCreateInfoEXT debug_info{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback
        };

        VkInstanceCreateInfo create_info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = &debug_info,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<u32>(requested_layers.size()),
            .ppEnabledLayerNames = requested_layers.data(),
            .enabledExtensionCount = static_cast<u32>(requested_extensions.size()),
            .ppEnabledExtensionNames = requested_extensions.data()
        };

        if (vkCreateInstance(&create_info, nullptr, &m_vulkan_instance) != VK_SUCCESS) return false;

        volk_load_instance(m_vulkan_instance);
        return true;
    }

    bool Application::init_volk() {
        return true;
    }

    void Application::volk_load_instance(VkInstance instance) {

    }

    void Application::volk_finalize() {

    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Application::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data
    ) {
        if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "Validation Layer: " << callback_data->pMessage << std::endl;
        }

        return VK_FALSE;
    }

    ErrorCode Application::run() {
        while(!m_window.should_close()) {
            glfwPollEvents();
        }

        return ErrorCode::OK;
    }
}