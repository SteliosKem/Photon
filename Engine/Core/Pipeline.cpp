#include "Pipeline.h"
#include "Common.h"
#include <iostream>

namespace Photon {
    Pipeline::Pipeline(const Filepath& vertex_shader, const Filepath& fragment_shader) {
        create_graphics_pipeline(vertex_shader, fragment_shader);
    }

    Pipeline::~Pipeline() {}

    void Pipeline::create_graphics_pipeline(const Filepath& vertex_shader, const Filepath& fragment_shader) {
        string vertex_code = read_file(vertex_shader);
        string fragment_code = read_file(fragment_shader);

        std::cout << "Loaded Shaders: " << vertex_code.size() << ", " << fragment_code.size() << '\n';
    }
}