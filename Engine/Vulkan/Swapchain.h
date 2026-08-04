#pragma once
#include "Core/Common.h"
#include "Context.h"

struct VkSwapchainKHR_T;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
struct VkImage_T;
typedef struct VkImage_T* VkImage;
struct VkImageView_T;
typedef struct VkImageView_T* VkImageView;
struct VkSemaphore_T;
typedef struct VkSemaphore_T* VkSemaphore;
struct VmaAllocation_T;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Photon {
	class Swapchain {
	public:
		Swapchain() = delete;
		Swapchain(shared_ptr<VulkanContext> context);

		~Swapchain();

		bool create(u32 width, u32 height);
		void destroy();
		void recreate(u32 width, u32 height);

		VkSwapchainKHR& get();
		VkImage depth_image();
		VkImageView depth_image_view();
		vector<VkImage>& images();
		vector<VkImageView>& image_views();
		vector<VkSemaphore>& render_complete_semaphores();

		u32 width() const;
		u32 height() const;

		ErrorCode ok() const;
	private:
		shared_ptr<VulkanContext> m_context{ nullptr };

		VkSwapchainKHR m_swapchain{ nullptr };
		vector<VkImage> m_images{};
		vector<VkImageView> m_image_views{};
		vector<VkSemaphore> m_render_complete_semaphores{};

		u32 m_swapchain_width{};
		u32 m_swapchain_height{};

		VkImage m_depth_image{ nullptr };
		VkImageView m_depth_image_view{ nullptr };
		VmaAllocation m_depth_image_allocation{ nullptr };

		bool m_error{ false };
	};
}