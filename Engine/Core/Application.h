#pragma once

#include "Window.h"
#include "Pipeline.h"

#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"

struct VmaAllocator_T;
typedef struct VmaAllocator_T* VmaAllocator;
struct VmaAllocation_T;
typedef struct VmaAllocation_T* VmaAllocation;
struct VkCommandPool_T;
typedef struct VkCommandPool_T* VkCommandPool;
struct VkCommandBuffer_T;
typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Photon {
    struct ApplicationInfo {
        string name;
        Version version;
        WindowInfo main_window_info;
    };

    struct FrameResources {
        VkCommandPool command_pool{ nullptr };
        VkCommandBuffer command_buffer{ nullptr };
        VkSemaphore image_acquired_semaphore{ nullptr };
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

        bool init_vulkan();
        bool create_vulkan_instance();
        bool create_surface();
        bool find_graphics_queue();
        bool create_device();
        bool init_vma();
        bool create_swapchain(u32 width, u32 height);
        bool create_sync_resources();
        bool create_command_buffers();

        void render();

        void destroy_swapchain();

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

        VkInstance m_vulkan_instance{ nullptr };
        VkSurfaceKHR m_surface{ nullptr };
        VkPhysicalDevice m_physical_device{ nullptr };
        u32 m_graphics_queue_family_index{ UINT32_MAX };
        VkQueue m_graphics_queue{ nullptr };
        VkDevice m_device{ nullptr };
        VmaAllocator m_vma_allocator{ nullptr };

        VkSwapchainKHR m_swapchain{ nullptr };
        vector<VkImage> m_swapchain_images{};
        vector<VkImageView> m_swapchin_image_views{};
        vector<VkSemaphore> m_render_complete_semaphores{};

        VkImage m_depth_image{ nullptr };
        VkImageView m_depth_image_view{ nullptr };
        VmaAllocation m_depth_image_allocation{ nullptr };

        shared_ptr<Pipeline> m_pipeline;

        u32 m_width{ 800 };
        u32 m_height{ 600 };

        u32 m_swapchain_width{};
        u32 m_swapchain_height{};

        bool m_require_swapchain_recreate{ false };

        constexpr static u32 VULKAN_VERSION{ VK_API_VERSION_1_4 };
        constexpr static u32 MAX_FRAMES_IN_FLIGHT{ 2 };
        constexpr static VkFormat SWAPCHAIN_FORMAT{ VK_FORMAT_B8G8R8A8_SRGB };
        constexpr static VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };

        VkSemaphore m_timeline_semaphore{ nullptr };
        array<FrameResources, MAX_FRAMES_IN_FLIGHT> m_frame_resources;
    };
}