#pragma once

#include "Window.h"
#include "Pipeline.h"

namespace Photon {
    struct ApplicationInfo {
        string name;
        Version version;
        WindowInfo main_window_info;
    };

    class Application {
    public:
        Application() = delete;
        Application(const ApplicationInfo& app_info);

        ~Application();

        ErrorCode run();
    private:
        bool init();
        void shutdown();

        // Vulkan Initialization
        bool init_vulkan();
        bool create_vulkan_instance();
        bool init_volk();
        bool create_surface();
        bool find_graphics_queue();

        void volk_load_instance(VkInstance instance);
        void volk_finalize();

        VkPhysicalDevice find_physical_device();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
            void* user_data
        );
    private:
        Window m_window;
        ApplicationInfo m_app_info;
        Pipeline m_pipeline;

        VkInstance m_vulkan_instance{ nullptr };
        VkSurfaceKHR m_surface{ nullptr };
        VkPhysicalDevice m_physical_device{ nullptr };
        u32 m_graphics_queue_family_index{ UINT32_MAX };
        VkQueue m_graphics_queue{ nullptr };

        constexpr static u32 VULKAN_VERSION{ VK_API_VERSION_1_4 };
        constexpr static u32 MAX_FRAMES_IN_FLIGHT{ 2 };
        constexpr static VkFormat SWAPCHAIN_FORMAT{ VK_FORMAT_B8G8R8A8_SRGB };
        constexpr static VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };
    };
}