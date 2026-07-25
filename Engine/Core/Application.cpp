#include "Application.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Photon {
    Application::Application(const ApplicationInfo& app_info)
        : m_version{ app_info.version }, m_window(app_info.main_window_info) {
        init();
    }

    Application::~Application() {
        shutdown();
    }

    void Application::init() {
        glfwInit();
        m_window.init_window();
    }

    void Application::shutdown() {
        glfwTerminate();
    }

    ErrorCode Application::run() {
        while(!m_window.should_close()) {
            glfwPollEvents();
        }

        return ErrorCode::OK;
    }
}