#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Photon {
    Window::Window(const WindowInfo& window_info) : m_info{ window_info } {
        init_window();
    }

    Window::~Window() {
        destroy_window();
    }

    void Window::init_window() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, m_info.resizable);

        m_window = glfwCreateWindow(m_info.width, m_info.height, m_info.title.c_str(), nullptr, nullptr);
    }

    void Window::destroy_window() {
        glfwDestroyWindow(m_window);
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(m_window);
    }
}