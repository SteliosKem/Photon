#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include <array>

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;
using std::vector;
using std::array;
using Filepath = std::filesystem::path;

using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float_t;

namespace Photon {
    struct Version {
        u8 a{ 1 };
        u8 b{ 0 };
        u8 c{ 0 };
    };

    enum class ErrorCode {
        OK,
        GENERAL_ERROR
    };

    struct WindowInfo {
        u32 width{ 800 };
        u32 height{ 600 };
        string title{ "Photon Engine" };
        bool resizable{ true };
    };

    struct ApplicationInfo {
        string name;
        Version version;
        WindowInfo main_window_info;
    };

    string read_file(const Filepath& path);
}