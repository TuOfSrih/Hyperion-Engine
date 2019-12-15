
#include "Shader.hpp"

#include "System/IO.hpp"
#include "RenderContext.hpp"

namespace Hyperion::Rendering {

	std::string Shader::shaderDir = ".";
	
	Shader::Shader(std::string name)
	{
		std::filesystem::path path = std::filesystem::current_path() / shaderDir / name;

		std::vector<std::byte> shaderCode = System::IO::readBinaryFile(path.string());

		vk::ShaderModuleCreateInfo shaderInfo{
			{},
			shaderCode.size(),
			reinterpret_cast<uint32_t*>(shaderCode.data())
		};

		module = RenderContext::active->getDevice().createShaderModule(shaderInfo);
	}
}