#include <iostream>
#include <vulkan/vulkan.h>
#include "Core/Application.h"

using namespace Photon;

int main() {
    ApplicationInfo app_info{};
    app_info.name = "Photon Engine";
    app_info.version = Version{1, 0 ,0};

    WindowInfo main_win_info{};
    main_win_info.title = "Photon Engine";
    app_info.main_window_info = main_win_info;

    Application app(app_info);

    if(app.run() != ErrorCode::OK) return 1;

    return 0;
}