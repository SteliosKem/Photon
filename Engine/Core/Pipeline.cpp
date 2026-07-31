#include "Pipeline.h"
#include "Common.h"
#include "Logging.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Photon {
    constexpr static VkFormat SWAPCHAIN_FORMAT{ VK_FORMAT_B8G8R8A8_SRGB };
    constexpr static VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };

    Pipeline::Pipeline(const VkDevice& device, const string& vertex_path, const string& fragment_path) {
        if (!create_shaders(device, vertex_path, fragment_path)) {
            Logger::error("Failed to create shaders.");
        }

        Logger::info("Created shaders.");
        create_graphics_pipeline(device);
    }

    Pipeline::~Pipeline() {}

    bool Pipeline::create_shaders(const VkDevice& device, const string& vertex_path, const string& fragment_path) {
        if (m_vertex_shader = make_shared<Shader>(vertex_path, ShaderType::VERTEX, device); !m_vertex_shader->exists()) return false;
        if (m_fragment_shader = make_shared<Shader>(fragment_path, ShaderType::FRAGMENT, device); !m_fragment_shader->exists()) return false;
        return true;
    }

    void Pipeline::create_graphics_pipeline(const VkDevice& device) {
        VkPipelineLayoutCreateInfo pipeline_layout_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pushConstantRangeCount = 0
        };
        if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &m_layout) != VK_SUCCESS) {
            Logger::error("Failed to create the pipeline layout.");
        }

        const char* entrypoint = "main";
        vector<VkPipelineShaderStageCreateInfo> shader_stages{
            VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = m_vertex_shader->get_vk_shader(),
                .pName = entrypoint
            },
            VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = m_fragment_shader->get_vk_shader(),
                .pName = entrypoint
            }
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        };

        VkPipelineDepthStencilStateCreateInfo depth_stencil_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .stencilTestEnable = VK_FALSE
        };

        VkPipelineViewportStateCreateInfo viewport_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr
        };

        VkPipelineRasterizationStateCreateInfo raster_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo multisample_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
        };

        VkPipelineColorBlendAttachmentState attach_state{
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo blend_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &attach_state
        };

        vector<VkDynamicState> dynamic_state{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamic_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<u32>(dynamic_state.size()),
            .pDynamicStates = dynamic_state.data()
        };

        VkPipelineRenderingCreateInfo render_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &SWAPCHAIN_FORMAT,
            .depthAttachmentFormat = DEPTH_FORMAT
        };

        VkGraphicsPipelineCreateInfo pipeline_info{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &render_info,
            .stageCount = static_cast<u32>(shader_stages.size()),
            .pStages = shader_stages.data(),
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &input_assembly_info,
            .pViewportState = &viewport_info,
            .pRasterizationState = &raster_info,
            .pMultisampleState = &multisample_info,
            .pDepthStencilState = &depth_stencil_info,
            .pColorBlendState = &blend_info,
            .pDynamicState = &dynamic_state_info,
            .layout = m_layout,
            .renderPass = VK_NULL_HANDLE
        };

        if (vkCreateGraphicsPipelines(device, nullptr, 1, &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS) {
            Logger::error("Error creating the graphics pipeline.");
        }
    }
}