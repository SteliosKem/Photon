#pragma once

#include "Core/Common.h"
#include "Core/Window.h"

struct VkInstance_T;
typedef struct VkInstance_T* VkInstance;
struct VkSurfaceKHR_T;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
struct VkPhysicalDevice_T;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
struct VkQueue_T;
typedef struct VkQueue_T* VkQueue;
struct VkDevice_T;
typedef struct VkDevice_T* VkDevice;
struct VmaAllocator_T;
typedef struct VmaAllocator_T* VmaAllocator;
struct VmaAllocation_T;
typedef struct VmaAllocation_T* VmaAllocation;

enum VkFormat;

namespace Photon {
	class VulkanContext {
    public:
        VulkanContext(Window& window, const ApplicationInfo& app_info);

        ~VulkanContext();

        ErrorCode ok() const;

        VkDevice device();
        VkQueue graphics_queue();
        VmaAllocator allocator();
        VkPhysicalDevice gpu();
        VkSurfaceKHR surface();
        u32 graphics_queue_family_index();

        void wait_idle();

        static const u32 VULKAN_VERSION;
        static const VkFormat SWAPCHAIN_FORMAT;
        static const VkFormat DEPTH_FORMAT;
    private:
        bool init();
        bool create_vulkan_instance();
        bool create_surface();
        bool find_graphics_queue();
        bool create_device();
        bool init_vma();

        VkPhysicalDevice find_physical_device();
	private:
        Window& m_window;
        const ApplicationInfo& m_app_info;

        VkInstance m_vulkan_instance{ nullptr };
        VkSurfaceKHR m_surface{ nullptr };
        VkPhysicalDevice m_physical_device{ nullptr };
        u32 m_graphics_queue_family_index{ UINT32_MAX };
        VkQueue m_graphics_queue{ nullptr };
        VkDevice m_device{ nullptr };
        VmaAllocator m_vma_allocator{ nullptr };

        bool m_error{ false };
	};
}