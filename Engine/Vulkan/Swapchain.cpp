#include "Swapchain.h"
#include "Context.h"
#include "Core/Window.h"
#include "Core/Logging.h"

#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Volk/volk.h>
#include <vma/vk_mem_alloc.h>

namespace Photon {
    Swapchain::Swapchain(shared_ptr<VulkanContext> context) {
        m_context = context;
    }

    bool Swapchain::create(u32 width, u32 height) {
        m_swapchain_width = width;
        m_swapchain_height = height;

        VkSurfaceCapabilitiesKHR surface_capabilities{};
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_context->gpu(), m_context->surface(), &surface_capabilities) != VK_SUCCESS) {
            Logger::error("Could not retrieve surface capabilities.");
            m_error = true;
            return false;
        }

        u32 requested_image_count{ std::max(2u, surface_capabilities.minImageCount) };
        if (surface_capabilities.maxImageCount > 0)
            requested_image_count = std::min(requested_image_count, surface_capabilities.maxImageCount);

        VkSwapchainCreateInfoKHR swapchain_create_info{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = m_context->surface(),
            .minImageCount = requested_image_count,
            .imageFormat = VulkanContext::SWAPCHAIN_FORMAT,
            .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
            .imageExtent{.width = m_swapchain_width, .height = m_swapchain_height },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR
        };

        if (vkCreateSwapchainKHR(m_context->device(), &swapchain_create_info, nullptr, &m_swapchain) != VK_SUCCESS) {
            Logger::error("Failed to create swapchain.");
            m_error = true;
            return false;
        }

        u32 image_count{ 0 };
        vkGetSwapchainImagesKHR(m_context->device(), m_swapchain, &image_count, nullptr);
        m_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_context->device(), m_swapchain, &image_count, m_images.data());
        m_image_views.resize(image_count);

        for (size_t i = 0; i < m_images.size(); i++) {
            VkImageViewCreateInfo img_view_info{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = m_images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = VulkanContext::SWAPCHAIN_FORMAT,
                .subresourceRange {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            if (vkCreateImageView(m_context->device(), &img_view_info, nullptr, &m_image_views[i]) != VK_SUCCESS) {
                Logger::error("Failed to create swapchain image view.");
                return false;
            }
        }

        m_render_complete_semaphores.resize(m_images.size());
        for (VkSemaphore& semaphore : m_render_complete_semaphores) {
            VkSemaphoreCreateInfo semaphore_info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            if (vkCreateSemaphore(m_context->device(), &semaphore_info, nullptr, &semaphore) != VK_SUCCESS) {
                Logger::error("Failed to create the render-complete semaphore.");
                m_error = true;
                return false;
            }
        }

        VkImageCreateInfo depth_create_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VulkanContext::DEPTH_FORMAT,
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

        if (vmaCreateImage(m_context->allocator(), &depth_create_info, &alloc_info, &m_depth_image,
            &m_depth_image_allocation, nullptr) != VK_SUCCESS) {
            Logger::error("Failed to allocate depth image.");
            m_error = true;
            return false;
        }

        VkImageViewCreateInfo depth_img_view_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_depth_image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VulkanContext::DEPTH_FORMAT,
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .levelCount = 1,
                .layerCount = 1
            }
        };

        if (vkCreateImageView(m_context->device(), &depth_img_view_info, nullptr, &m_depth_image_view) != VK_SUCCESS) {
            Logger::error("Failed to create depth image view");
            m_error = true;
            return false;
        }

        return true;
    }

    void Swapchain::destroy() {
        for (VkImageView swapchain_img_view : m_image_views)
            vkDestroyImageView(m_context->device(), swapchain_img_view, nullptr);
        m_image_views.clear();

        for (VkSemaphore& semaphore : m_render_complete_semaphores)
            vkDestroySemaphore(m_context->device(), semaphore, nullptr);
        m_render_complete_semaphores.clear();

        if (m_swapchain) {
            vkDestroySwapchainKHR(m_context->device(), m_swapchain, nullptr);
            m_swapchain = nullptr;
        }

        if (m_depth_image_view) {
            vkDestroyImageView(m_context->device(), m_depth_image_view, nullptr);
            vmaDestroyImage(m_context->allocator(), m_depth_image, m_depth_image_allocation);
            m_depth_image_view = nullptr;
        }
    }

    void Swapchain::recreate(u32 width, u32 height) {
        destroy();
        create(width, height);
    }

    VkSwapchainKHR& Swapchain::get() {
        return m_swapchain;
    }

    VkImage Swapchain::depth_image() {
        return m_depth_image;
    }
    VkImageView Swapchain::depth_image_view() {
        return m_depth_image_view;
    }
    vector<VkImage>& Swapchain::images() {
        return m_images;
    }
    vector<VkImageView>& Swapchain::image_views() {
        return m_image_views;
    }
    vector<VkSemaphore>& Swapchain::render_complete_semaphores() {
        return m_render_complete_semaphores;
    }
    u32 Swapchain::width() const {
        return m_swapchain_width;
    }
    u32 Swapchain::height() const {
        return m_swapchain_height;
    }

    Swapchain::~Swapchain() {
        destroy();
    }

    ErrorCode Swapchain::ok() const {
        return m_error ? ErrorCode::GENERAL_ERROR : ErrorCode::OK;
    }
}