#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Common.h"

namespace Photon {
    struct WindowInfo {
        u32 width{ 800 };
        u32 height{ 600 };
        string title{ "Photon Engine" };
        bool resizable{ true };
    };

    class Window {
    public:
        Window() = delete;
        Window(const WindowInfo& window_info);

        ~Window();

        bool should_close() const;
    private:
        friend class Application;

        void init_window();
        void destroy_window();
    private:
        WindowInfo m_info;
        GLFWwindow* m_window;
    };
}