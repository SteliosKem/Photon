#pragma once

#include "Window.h"
#include "Vulkan/Swapchain.h"
#include "Vulkan/Context.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Shader.h"
#include "Vulkan/Renderer.h"

namespace Photon {
    class Application {
    public:
        Application() = delete;
        Application(const ApplicationInfo& app_info);

        ~Application();

        ErrorCode run();
    private:
        bool init();
        void shutdown();
    private:
        Window m_window;
        ApplicationInfo m_app_info;

        shared_ptr<VulkanContext> m_vk_context{ nullptr };
        shared_ptr<Swapchain> m_swapchain{ nullptr };
        shared_ptr<Pipeline> m_pipeline{ nullptr };
        unique_ptr<Renderer> m_renderer{ nullptr };
    };
}