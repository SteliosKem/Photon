#pragma once

#include "Core/Common.h"

struct VkShaderModule_T;
typedef struct VkShaderModule_T* VkShaderModule;
struct VkDevice_T;
typedef struct VkDevice_T* VkDevice;

namespace Photon {
	enum class ShaderType {
		FRAGMENT,
		VERTEX,
		COMPUTE
	};

	class Shader {
	public:
		Shader() = delete;
		Shader(const Filepath& path, ShaderType type, VkDevice device);

		~Shader();

		bool exists() const;
		VkShaderModule get_vk_shader();
	private:
		VkShaderModule m_shader{ nullptr };
		VkDevice m_device{ nullptr };
		ShaderType m_type{};
	};
}