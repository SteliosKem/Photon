#pragma once
#include "Common.h"

namespace Photon {
    class Pipeline {
    public:
        Pipeline() = delete;
        Pipeline(const Filepath& vertex_shader, const Filepath& fragment_shader);

        ~Pipeline();
    private:
        void create_graphics_pipeline(const Filepath& vertex_shader, const Filepath& fragment_shader);
    };
}