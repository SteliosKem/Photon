#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Photon {
    void GLFWContext::init() {
        glfwInit();
    }

    void GLFWContext::terminate() {
        glfwTerminate();
    }

    Window::Window(const WindowInfo& window_info) : m_info{ window_info } {}

    Window::~Window() {
        destroy();
    }

    void Window::init() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, m_info.resizable);

        m_window = glfwCreateWindow(m_info.width, m_info.height, m_info.title.c_str(), nullptr, nullptr);
    }

    void Window::destroy() {
        glfwDestroyWindow(m_window);
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(m_window);
    }

    GLFWwindow* Window::get_native() {
        return m_window;
    }

    void Window::poll() {
        glfwPollEvents();
    }

    u32 Window::width() const {
        return m_info.width;
    }
    u32 Window::height() const {
        return m_info.height;
    }
}