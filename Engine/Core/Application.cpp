#include "Application.h"
#include "Logging.h"

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

        if (!init_vulkan()) return false;


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
            Logger::error("Failed to create Vulkan instance.");
            return false;
        }

        Logger::info("Initialized Vulkan instance.");

        if (!create_surface()) {
            Logger::error("Failed to create surface.");
            return false;
        }

        Logger::info("Created surface.");
        
        if (m_physical_device = find_physical_device(); !m_physical_device) {
            Logger::error("No appropriate physical device found.");
            return false;
        }

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);

        Logger::info("Found device: " + string(properties.deviceName));

        if (!find_graphics_queue()) {
            Logger::error("No compatible graphics queue found.");
            return false;
        }

        Logger::info("Found graphics queue.");
    }

    bool Application::create_vulkan_instance() {
        if (!init_volk()) {
            Logger::error("Failed to initialize Volk.");
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

    bool Application::create_surface() {
        if (!glfwCreateWindowSurface(m_vulkan_instance, m_window.m_window, nullptr, &m_surface)) return true;
        return false;
    }

    bool Application::find_graphics_queue() {
        u32 queue_family_count{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties2(m_physical_device, &queue_family_count, nullptr);
        vector<VkQueueFamilyProperties2> family_props(queue_family_count, { .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 });
        vkGetPhysicalDeviceQueueFamilyProperties2(m_physical_device, &queue_family_count, family_props.data());

        for (u32 family_index{ 0 }; family_index < queue_family_count; family_index++) {
            VkBool32 has_presentation_support{ VK_FALSE };
            vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, family_index, m_surface, &has_presentation_support);

            const auto& props = family_props[family_index];
            if (props.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && has_presentation_support) {
                m_graphics_queue_family_index = family_index;
                return true;
            }
        }
        return false;
    }

    void Application::volk_load_instance(VkInstance instance) {

    }

    void Application::volk_finalize() {

    }


    VkPhysicalDevice Application::find_physical_device() {
        u32 device_count{ 0 };
        vkEnumeratePhysicalDevices(m_vulkan_instance, &device_count, nullptr);
        vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_vulkan_instance, &device_count, devices.data());

        VkPhysicalDevice device{ nullptr };
        if (device_count) {
            device = devices[0];
            for (VkPhysicalDevice& dev : devices) {
                VkPhysicalDeviceProperties properties{};
                vkGetPhysicalDeviceProperties(dev, &properties);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    device = dev;
                    break;
                }
            }
        }
        return device;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Application::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data
    ) {
        if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            Logger::error("Validation Layer: " + string(callback_data->pMessage));
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