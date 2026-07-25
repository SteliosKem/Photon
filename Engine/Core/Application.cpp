#include "Application.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const Filepath VERTEX_PATH = "Shaders/basicVertex.vert.spv";
const Filepath FRAGMENT_PATH = "Shaders/basicFragment.frag.spv";

namespace Photon {
    Application::Application(const ApplicationInfo& app_info)
        : m_version{ app_info.version }, m_window(app_info.main_window_info)
        , m_pipeline(VERTEX_PATH, FRAGMENT_PATH) {
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