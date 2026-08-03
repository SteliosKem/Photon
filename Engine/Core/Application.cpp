#include "Application.h"
#include "Logging.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

const Filepath VERTEX_PATH = "Shaders/basicVertex.vert";
const Filepath FRAGMENT_PATH = "Shaders/basicFragment.frag";

namespace Photon {
    Application::Application(const ApplicationInfo& app_info)
        : m_app_info{ app_info }, m_window(app_info.main_window_info) {
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
        vkDeviceWaitIdle(m_device);

        if (m_timeline_semaphore)
            vkDestroySemaphore(m_device, m_timeline_semaphore, nullptr);

        for (auto& res : m_frame_resources) {
            vkDestroySemaphore(m_device, res.image_acquired_semaphore, nullptr);
            vkDestroyCommandPool(m_device, res.command_pool, nullptr);
        }

        m_pipeline.reset();

        destroy_swapchain();

        if (m_vma_allocator)
            vmaDestroyAllocator(m_vma_allocator);

        if (m_surface)
            vkDestroySurfaceKHR(m_vulkan_instance, m_surface, nullptr);

        if (m_device)
            vkDestroyDevice(m_device, nullptr);

        if (m_vulkan_instance)
            vkDestroyInstance(m_vulkan_instance, nullptr);
        volkFinalize();

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

        if (!create_device()) {
            Logger::error("Failed to create logical device.");
            return false;
        }

        Logger::info("Created logical device.");

        if (!init_vma()) {
            Logger::error("Failed to create Vulkan Memory Allocator.");
            return false;
        }

        Logger::info("Created Vulkan Memory Allocator.");

        if (!create_swapchain(m_width, m_height)) {
            Logger::error("Failed to create swapchain.");
            return false;
        }

        Logger::info("Created swapchain.");

        m_pipeline = make_shared<Pipeline>(m_device, VERTEX_PATH, FRAGMENT_PATH);
        
        if (!create_sync_resources()) {
            Logger::error("Failed to create sync related resources");
            return;
        }

        if (!create_command_buffers()) {
            Logger::error("Failed to create command buffer objects");
            return false;
        }

        return true;
    }

    bool Application::create_vulkan_instance() {
        if (volkInitialize() != VK_SUCCESS) {
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

        volkLoadInstance(m_vulkan_instance);
        return true;
    }

    bool Application::create_surface() {
        return glfwCreateWindowSurface(m_vulkan_instance, m_window.m_window, nullptr, &m_surface) == VK_SUCCESS;
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

    bool Application::create_device() {
        VkPhysicalDeviceVulkan14Features supported_features_14{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
            .pNext = nullptr
        };
        VkPhysicalDeviceVulkan13Features supported_features_13{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = &supported_features_14
        };
        VkPhysicalDeviceVulkan12Features supported_features_12{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = &supported_features_13
        };
        VkPhysicalDeviceFeatures2 supported_features {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &supported_features_12
        };
        vkGetPhysicalDeviceFeatures2(m_physical_device, &supported_features);

        if (!supported_features_13.dynamicRendering || !supported_features_13.synchronization2
            || !supported_features_12.timelineSemaphore) {
            Logger::error("Physical device does not meet feature requirements.");
            return false;
        }

        VkPhysicalDeviceVulkan14Features features_14{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
            .pNext = nullptr
        };
        VkPhysicalDeviceVulkan13Features features_13{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = &features_14,
            .synchronization2 = VK_TRUE,
            .dynamicRendering = VK_TRUE
        };
        VkPhysicalDeviceVulkan12Features features_12{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = &features_13,
            .timelineSemaphore = VK_TRUE
        };
        VkPhysicalDeviceFeatures2 features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &features_12
        };

        vector<f32> queue_priorities{ 1.0f };
        VkDeviceQueueCreateInfo graphics_queue_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = m_graphics_queue_family_index,
            .queueCount = 1,
            .pQueuePriorities = queue_priorities.data()
        };

        const vector<const char*> device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        VkDeviceCreateInfo device_create_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &features,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &graphics_queue_info,
            .enabledExtensionCount = static_cast<u32>(device_extensions.size()),
            .ppEnabledExtensionNames = device_extensions.data(),
            .pEnabledFeatures = nullptr
        };

        if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
            return false;

        volkLoadDevice(m_device);

        vkGetDeviceQueue(m_device, m_graphics_queue_family_index, 0, &m_graphics_queue);
        if (!m_graphics_queue) {
            Logger::error("Could not retrieve graphics queue.");
            return false;
        }
        return true;
    }

    bool Application::init_vma() {
        VmaVulkanFunctions vma_function_info{};
        VmaAllocatorCreateInfo vma_allocator_info{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = m_physical_device,
            .device = m_device,
            .pVulkanFunctions = &vma_function_info,
            .instance = m_vulkan_instance,
            .vulkanApiVersion = VULKAN_VERSION
        };

        vmaImportVulkanFunctionsFromVolk(&vma_allocator_info, &vma_function_info);

        if (vmaCreateAllocator(&vma_allocator_info, &m_vma_allocator) != VK_SUCCESS) return false;
        return true;
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

        u32 format_count{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);
        vector<VkSurfaceFormatKHR> surface_formats(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, surface_formats.data());

        bool format_supported{ false };
        for (const VkSurfaceFormatKHR& surface_format : surface_formats) {
            if (surface_format.format == SWAPCHAIN_FORMAT) {
                format_supported = true;
                break;
            }
        }
        if (!format_supported) {
            Logger::error("Requested swapchain format is not supported by the surface.");
            return nullptr;
        }

        return device;
    }

    bool Application::create_swapchain(u32 width, u32 height) {
        m_swapchain_width = width;
        m_swapchain_height = height;

        VkSurfaceCapabilitiesKHR surface_capabilities{};
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &surface_capabilities) != VK_SUCCESS) {
            Logger::error("Could not retrieve surface capabilities.");
            return false;
        }

        u32 requested_image_count{ std::max(2u, surface_capabilities.minImageCount) };
        if (surface_capabilities.maxImageCount > 0)
            requested_image_count = std::min(requested_image_count, surface_capabilities.maxImageCount);
        
        VkSwapchainCreateInfoKHR swapchain_create_info{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = m_surface,
            .minImageCount = requested_image_count,
            .imageFormat = SWAPCHAIN_FORMAT,
            .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
            .imageExtent{.width = m_swapchain_width, .height = m_swapchain_height },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR
        };

        if (vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &m_swapchain) != VK_SUCCESS) {
            Logger::error("Failed to create swapchain.");
            return false;
        }

        u32 image_count{ 0 };
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);
        m_swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_swapchain_images.data());
        m_swapchin_image_views.resize(image_count);

        for (size_t i = 0; i < m_swapchain_images.size(); i++) {
            VkImageViewCreateInfo img_view_info{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = m_swapchain_images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = SWAPCHAIN_FORMAT,
                .subresourceRange {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            if (vkCreateImageView(m_device, &img_view_info, nullptr, &m_swapchin_image_views[i]) != VK_SUCCESS) {
                Logger::error("Failed to create swapchain image view.");
                return false;
            }
        }

        m_render_complete_semaphores.resize(m_swapchain_images.size());
        for (VkSemaphore& semaphore : m_render_complete_semaphores) {
            VkSemaphoreCreateInfo semaphore_info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &semaphore) != VK_SUCCESS) {
                Logger::error("Failed to create the render-complete semaphore.");
                return false;
            }
        }

        VkImageCreateInfo depth_create_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = DEPTH_FORMAT,
            .extent{
                .width = m_swapchain_width,
                .height = m_swapchain_height,
                .depth = 1
            },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo alloc_info{
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        if (vmaCreateImage(m_vma_allocator, &depth_create_info, &alloc_info, &m_depth_image,
            &m_depth_image_allocation, nullptr) != VK_SUCCESS) {
            Logger::error("Failed to allocate depth image.");
        }

        VkImageViewCreateInfo depth_img_view_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_depth_image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = DEPTH_FORMAT,
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .levelCount = 1,
                .layerCount = 1
            }
        };

        if (vkCreateImageView(m_device, &depth_img_view_info, nullptr, &m_depth_image_view) != VK_SUCCESS) {
            Logger::error("Failed to create depth image view");
            return false;
        }

        return true;
    }

    bool Application::create_sync_resources() {
        VkSemaphoreTypeCreateInfo semaphore_type_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = MAX_FRAMES_IN_FLIGHT
        };

        VkSemaphoreCreateInfo semaphore_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &semaphore_type_info
        };

        if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_timeline_semaphore) != VK_SUCCESS) {
            Logger::error("Unable to create the timeline semaphore.");
            return false;
        }

        for (FrameResources& res : m_frame_resources) {
            VkSemaphoreCreateInfo _semaphore_info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            if (vkCreateSemaphore(m_device, &_semaphore_info, nullptr, &res.image_acquired_semaphore) != VK_SUCCESS) {
                Logger::error("Error creating the per-frame image-acquire semaphore.");
                return false;
            }
        }
        return true;
    }

    bool Application::create_command_buffers() {
        for (FrameResources& res : m_frame_resources) {
            VkCommandPoolCreateInfo pool_info{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .queueFamilyIndex = m_graphics_queue_family_index
            };

            if (vkCreateCommandPool(m_device, &pool_info, nullptr, &res.command_pool) != VK_SUCCESS) {
                Logger::error("Failed to create command buffer pool.");
                return false;
            }

            VkCommandBufferAllocateInfo cmd_alloc_info{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = res.command_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };

            if (vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &res.command_buffer) != VK_SUCCESS) {
                Logger::error("Failed to allocate command buffer.");
                return false;
            }
        }
        return true;
    }

    void Application::destroy_swapchain() {
        for (VkImageView swapchain_img_view : m_swapchin_image_views)
            vkDestroyImageView(m_device, swapchain_img_view, nullptr);
        m_swapchin_image_views.clear();

        for (VkSemaphore& semaphore : m_render_complete_semaphores)
            vkDestroySemaphore(m_device, semaphore, nullptr);
        m_render_complete_semaphores.clear();

        if (m_swapchain) {
            vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            m_swapchain = nullptr;
        }

        if (m_depth_image_view) {
            vkDestroyImageView(m_device, m_depth_image_view, nullptr);
            vmaDestroyImage(m_vma_allocator, m_depth_image, m_depth_image_allocation);
            m_depth_image_view = nullptr;
        }
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

    void Application::render() {
        
    }

    ErrorCode Application::run() {
        while(!m_window.should_close()) {
            glfwPollEvents();
        }

        return ErrorCode::OK;
    }
}