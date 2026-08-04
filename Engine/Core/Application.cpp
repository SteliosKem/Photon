#include "Application.h"
#include "Logging.h"

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
        GLFWContext::init();
        m_window.init();

        m_vk_context = make_shared<VulkanContext>(m_window, m_app_info);
        if (m_vk_context->ok() == ErrorCode::GENERAL_ERROR) return false;

        m_swapchain = make_unique<Swapchain>(m_vk_context);
        if (!m_swapchain->create(m_window.width(), m_window.height())) {
            Logger::error("Failed to create swapchain.");
            return false;
        }

        Logger::info("Created swapchain.");

        m_pipeline = make_shared<Pipeline>(m_vk_context, VERTEX_PATH, FRAGMENT_PATH);
        m_renderer = make_unique<Renderer>(m_vk_context, m_swapchain, m_pipeline);
        if (m_renderer->ok() == ErrorCode::GENERAL_ERROR) return false;

        return true;
    }

    void Application::shutdown() {
        m_vk_context->wait_idle();

        m_renderer.reset();

        m_pipeline.reset();

        m_swapchain.reset();

        m_vk_context.reset();

        m_window.destroy();
        GLFWContext::terminate();
    }

    ErrorCode Application::run() {
        while(!m_window.should_close()) {
            m_window.poll();

            m_renderer->render();
        }

        return ErrorCode::OK;
    }
}