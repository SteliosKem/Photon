#pragma once

#include "Window.h"
#include "Pipeline.h"

namespace Photon {
    struct ApplicationInfo {
        string name;
        Version version;
        WindowInfo main_window_info;
    };

    class Application {
    public:
        Application() = delete;
        Application(const ApplicationInfo& app_info);

        ~Application();

        ErrorCode run();
    private:
        void init();
        void shutdown();
    private:
        Window m_window;
        Version m_version;

        Pipeline m_pipeline;
    };
}