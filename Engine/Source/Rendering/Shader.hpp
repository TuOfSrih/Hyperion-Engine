
#include "vulkan/vulkan.hpp"
#include <filesystem>

namespace Hyperion::Rendering {

	class Shader {

		vk::ShaderModule module;

	public:
		Shader(std::string name);

		static std::string shaderDir;

	};
}