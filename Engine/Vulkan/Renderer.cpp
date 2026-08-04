#include "Renderer.h"
#include "Context.h"
#include "Core/Logging.h"

#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Volk/volk.h>

namespace Photon {
    Renderer::Renderer(shared_ptr<VulkanContext> context, shared_ptr<Swapchain> swapchain, shared_ptr<Pipeline> pipeline)
        : m_context(context), m_swapchain(swapchain), m_pipeline(pipeline) {
        init();
    }

	void Renderer::init() {
        if (!create_sync_resources()) {
            Logger::error("Failed to create sync related resources");
            m_error = true; return;
        }

        if (!create_command_buffers()) {
            Logger::error("Failed to create command buffer objects");
            m_error = true; return;
        }
	}

    ErrorCode Renderer::ok() const { return m_error ? ErrorCode::GENERAL_ERROR : ErrorCode::OK; }

    bool Renderer::create_sync_resources() {
        VkSemaphoreTypeCreateInfo semaphore_type_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = MAX_FRAMES_IN_FLIGHT
        };

        VkSemaphoreCreateInfo semaphore_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &semaphore_type_info
        };

        if (vkCreateSemaphore(m_context->device(), &semaphore_info, nullptr, &m_timeline_semaphore) != VK_SUCCESS) {
            Logger::error("Unable to create the timeline semaphore.");
            return false;
        }

        for (FrameResources& res : m_frame_resources) {
            VkSemaphoreCreateInfo _semaphore_info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            if (vkCreateSemaphore(m_context->device(), &_semaphore_info, nullptr, &res.image_acquired_semaphore) != VK_SUCCESS) {
                Logger::error("Error creating the per-frame image-acquire semaphore.");
                return false;
            }
        }
        return true;
    }

    bool Renderer::create_command_buffers() {
        for (FrameResources& res : m_frame_resources) {
            VkCommandPoolCreateInfo pool_info{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .queueFamilyIndex = m_context->graphics_queue_family_index()
            };

            if (vkCreateCommandPool(m_context->device(), &pool_info, nullptr, &res.command_pool) != VK_SUCCESS) {
                Logger::error("Failed to create command buffer pool.");
                return false;
            }

            VkCommandBufferAllocateInfo cmd_alloc_info{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = res.command_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };

            if (vkAllocateCommandBuffers(m_context->device(), &cmd_alloc_info, &res.command_buffer) != VK_SUCCESS) {
                Logger::error("Failed to allocate command buffer.");
                return false;
            }
        }
        return true;
    }

    void Renderer::render() {
        if (m_require_swapchain_recreate) {
            vkDeviceWaitIdle(m_context->device());
            m_swapchain->recreate(m_width, m_height);
            m_require_swapchain_recreate = false;
        }

        const u32 frame_res_index{ m_frame_index++ % MAX_FRAMES_IN_FLIGHT };
        const u64 signal_value{ m_next_signal_value++ };
        const u64 wait_value{ signal_value - MAX_FRAMES_IN_FLIGHT };

        VkSemaphoreWaitInfo wait_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .semaphoreCount = 1,
            .pSemaphores = &m_timeline_semaphore,
            .pValues = &wait_value
        };
        vkWaitSemaphores(m_context->device(), &wait_info, UINT64_MAX);

        FrameResources& res = m_frame_resources[frame_res_index];
        vkResetCommandPool(m_context->device(), res.command_pool, 0);

        VkSemaphore image_acquire_semaphore = m_frame_resources[frame_res_index].image_acquired_semaphore;
        u32 image_index{ 0 };
        VkResult acquire_result = vkAcquireNextImageKHR(m_context->device(), m_swapchain->get(), UINT64_MAX, image_acquire_semaphore, VK_NULL_HANDLE, &image_index);

        if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
            m_require_swapchain_recreate = true;
            return;
        }
        else if (acquire_result == VK_SUBOPTIMAL_KHR)
            m_require_swapchain_recreate = true;

        VkCommandBufferBeginInfo cmd_begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };
        vkBeginCommandBuffer(res.command_buffer, &cmd_begin_info);

        vector<VkImageMemoryBarrier2> layout_barriers{
            VkImageMemoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .image = m_swapchain->images()[image_index],
                .subresourceRange {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            },
            VkImageMemoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
                .srcAccessMask = 0,
                .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                .image = m_swapchain->depth_image(),
                .subresourceRange {
                    .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            }
        };
        VkDependencyInfo dependency_info{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = static_cast<u32>(layout_barriers.size()),
            .pImageMemoryBarriers = layout_barriers.data()
        };
        vkCmdPipelineBarrier2(res.command_buffer, &dependency_info);

        VkRenderingAttachmentInfo color_attachment_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = m_swapchain->image_views()[image_index],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue{.color{0.01f, 0.01f, 0.01f, 1}}
        };

        VkRenderingAttachmentInfo depth_attachment_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = m_swapchain->depth_image_view(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue{.depthStencil{1.0f, 0}}
        };

        VkRenderingInfo rendering_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea{
                .offset{.x = 0, .y = 0},
                .extent{.width = m_swapchain->width(), .height = m_swapchain->height()}
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_info,
            .pDepthAttachment = &depth_attachment_info
        };

        vkCmdBeginRendering(res.command_buffer, &rendering_info);

        // Begin Rendering

        VkViewport viewport{
            .x = 0,
            .y = static_cast<f32>(m_swapchain->height()),
            .width = static_cast<f32>(m_swapchain->width()),
            .height = -static_cast<f32>(m_swapchain->height())
        };
        vkCmdSetViewport(res.command_buffer, 0, 1, &viewport);

        VkRect2D scissor{
            .offset{.x = 0, .y = 0},
            .extent{.width = m_swapchain->width(), .height = m_swapchain->height()}
        };
        vkCmdSetScissor(res.command_buffer, 0, 1, &scissor);
        vkCmdBindPipeline(res.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());
        vkCmdDraw(res.command_buffer, 3, 1, 0, 0);

        // End rendering

        vkCmdEndRendering(res.command_buffer);

        VkImageMemoryBarrier2 present_layout_barrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = m_swapchain->images()[image_index],
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        VkDependencyInfo present_dependency_info{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &present_layout_barrier
        };
        vkCmdPipelineBarrier2(res.command_buffer, &present_dependency_info);
        vkEndCommandBuffer(res.command_buffer);

        VkSemaphoreSubmitInfo image_acquire_wait_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = image_acquire_semaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        vector<VkSemaphoreSubmitInfo> semaphore_signals{
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = m_swapchain->render_complete_semaphores()[image_index],
                .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
            },
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                .semaphore = m_timeline_semaphore,
                .value = signal_value,
                .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
            }
        };

        VkCommandBufferSubmitInfo cmd_submit_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = res.command_buffer
        };
        VkSubmitInfo2 submit_info{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &image_acquire_wait_info,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &cmd_submit_info,
            .signalSemaphoreInfoCount = static_cast<u32>(semaphore_signals.size()),
            .pSignalSemaphoreInfos = semaphore_signals.data()
        };
        vkQueueSubmit2(m_context->graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);

        VkPresentInfoKHR present_info{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &m_swapchain->render_complete_semaphores()[image_index],
            .swapchainCount = 1,
            .pSwapchains = &(m_swapchain->get()),
            .pImageIndices = &image_index,
            .pResults = nullptr
        };

        vkQueuePresentKHR(m_context->graphics_queue(), &present_info);
    }

    Renderer::~Renderer() {
        if (m_timeline_semaphore)
            vkDestroySemaphore(m_context->device(), m_timeline_semaphore, nullptr);

        for (auto& res : m_frame_resources) {
            vkDestroySemaphore(m_context->device(), res.image_acquired_semaphore, nullptr);
            vkDestroyCommandPool(m_context->device(), res.command_pool, nullptr);
        }
    }
}