#pragma once
#include "Core/Common.h"
#include "Shader.h"
#include "Context.h"

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
        Pipeline(shared_ptr<VulkanContext> context, const Filepath& vertex_path, const Filepath& fragment_path);

        VkPipeline get();

        ~Pipeline();

        ErrorCode ok() const;
    private:
        bool create_shaders(const Filepath& vertex_path, const Filepath& fragment_path);
        void create_graphics_pipeline();
    private:
        shared_ptr<VulkanContext> m_context{ nullptr };

        shared_ptr<Shader> m_vertex_shader{ nullptr };
        shared_ptr<Shader> m_fragment_shader{ nullptr };

        VkPipelineLayout m_layout{ nullptr };
        VkPipeline m_pipeline{ nullptr };

        bool m_error{ false };
    };
}