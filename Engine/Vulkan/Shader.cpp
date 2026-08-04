#include "Shader.h"
#include "Core/Logging.h"

#include <shaderc/shaderc.hpp>

#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Volk/volk.h>
#include <vma/vk_mem_alloc.h>

namespace Photon {
	static shaderc_shader_kind to_shaderc(ShaderType type) {
		switch (type) {
		case ShaderType::FRAGMENT: return shaderc_fragment_shader;
		case ShaderType::VERTEX: return shaderc_vertex_shader;
		case ShaderType::COMPUTE: return shaderc_compute_shader;
		default: Logger::error("Shader type is not implemented");
		}
	}

	Shader::Shader(const Filepath& path, ShaderType type, VkDevice device) {
		const string source = read_file(path);
		m_device = device;
		m_type = type;
		if (source.empty())
			Logger::error("Specified shader file does not exist: " + path.string());

		Logger::info("Compiling shader: " + path.string());
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
		options.SetTargetSpirv(shaderc_spirv_version_1_6);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::CompilationResult res{compiler.CompileGlslToSpv(source, to_shaderc(type), path.filename().string().c_str(), options)};

		if (res.GetCompilationStatus() != shaderc_compilation_status_success)
			Logger::error("Shader compilation error: " + res.GetErrorMessage());

		vector<u32> spv{ res.cbegin(), res.cend() };
		VkShaderModuleCreateInfo module_create_info{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = spv.size() * sizeof(u32),
			.pCode = spv.data()
		};


		if (vkCreateShaderModule(device, &module_create_info, nullptr, &m_shader) != VK_SUCCESS) {
			Logger::error("Failed to create shader module.");
		}
	}

	Shader::~Shader() {
		if (m_shader) vkDestroyShaderModule(m_device, m_shader, nullptr);
	}

	bool Shader::exists() const { return m_shader != nullptr; }
	VkShaderModule Shader::get_vk_shader() { return m_shader; }
}