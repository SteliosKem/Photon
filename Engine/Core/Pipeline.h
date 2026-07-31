#pragma once
#include "Common.h"
#include "Shader.h"

struct VkDevice_T;
typedef struct VkDevice_T* VkDevice;
struct VkPipelineLayout_T;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
struct VkPipeline_T;
typedef struct VkPipeline_T* VkPipeline;

namespace Photon {
    class Pipeline {
    public:
        Pipeline() = delete;
        Pipeline(VkDevice device, const Filepath& vertex_path, const Filepath& fragment_path);

        ~Pipeline();
    private:
        bool create_shaders(VkDevice device, const Filepath& vertex_path, const Filepath& fragment_path);
        void create_graphics_pipeline(VkDevice device);
    private:
        shared_ptr<Shader> m_vertex_shader{ nullptr };
        shared_ptr<Shader> m_fragment_shader{ nullptr };

        VkPipelineLayout m_layout{ nullptr };
        VkPipeline m_pipeline{ nullptr };
        VkDevice m_device{ nullptr };
    };
}