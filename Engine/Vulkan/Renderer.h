#pragma once
#include "Core/Common.h"
#include "Context.h"
#include "Swapchain.h"
#include "Pipeline.h"

struct VkSemaphore_T;
typedef struct VkSemaphore_T* VkSemaphore;
struct VkCommandPool_T;
typedef struct VkCommandPool_T* VkCommandPool;
struct VkCommandBuffer_T;
typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Photon {
    struct FrameResources {
        VkCommandPool command_pool{ nullptr };
        VkCommandBuffer command_buffer{ nullptr };
        VkSemaphore image_acquired_semaphore{ nullptr };
    };

	class Renderer {
    public:
        Renderer() = delete;
        Renderer(shared_ptr<VulkanContext> context, shared_ptr<Swapchain> swapchain, shared_ptr<Pipeline> pipeline);

        ~Renderer();

        void render();
        ErrorCode ok() const;
    private:
        void init();
        bool create_sync_resources();
        bool create_command_buffers();
	private:
        constexpr static u32 MAX_FRAMES_IN_FLIGHT{ 2 };

        shared_ptr<VulkanContext> m_context{ nullptr };
        shared_ptr<Swapchain> m_swapchain{ nullptr };
        shared_ptr<Pipeline> m_pipeline{ nullptr };

        u32 m_width{ 800 };
        u32 m_height{ 600 };

        bool m_require_swapchain_recreate{ false };

        VkSemaphore m_timeline_semaphore{ nullptr };
        array<FrameResources, MAX_FRAMES_IN_FLIGHT> m_frame_resources;
        uint64_t m_frame_index = 0;
        uint64_t m_next_signal_value = MAX_FRAMES_IN_FLIGHT + 1;

        bool m_error{ false };
	};
}