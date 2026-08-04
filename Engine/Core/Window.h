#pragma once

#include "Common.h"

struct GLFWwindow;

namespace Photon {
    class GLFWContext {
    public:
        static void init();
        static void terminate();
    };

    class Window {
    public:
        Window() = delete;
        Window(const WindowInfo& window_info);

        ~Window();

        void poll();
        bool should_close() const;
        GLFWwindow* get_native();
        u32 width() const;
        u32 height() const;

        void init();
        void destroy();
    private:
        WindowInfo m_info;
        GLFWwindow* m_window;
    };
}