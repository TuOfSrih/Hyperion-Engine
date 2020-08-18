
#include "Shader.hpp"

#include "System/IO.hpp"
#include "RenderContext.hpp"

namespace Hyperion::Rendering {
	
	Shader::Shader(const std::filesystem::path& path)
	{
		std::vector<std::byte> shaderCode = System::IO::readBinaryFile(path.string());

		vk::ShaderModuleCreateInfo shaderInfo{
			{},
			shaderCode.size(),
			reinterpret_cast<uint32_t*>(shaderCode.data())
		};

		module = RenderContext::active->getDevice().createShaderModule(shaderInfo);
	}

	Shader::~Shader()
	{
		if (module != vk::ShaderModule()) {

			RenderContext::active->getDevice().destroyShaderModule(module);
		}
	}

	Shader::Shader(Shader&& other) {

		*this = std::move(other);
	}

	Shader& Shader::operator=(Shader&& other) {

		this->~Shader();

		module = std::move(other.module);

		return *this;
	}
	const vk::ShaderModule& Shader::getModule() const
	{
		ASSERT(module != vk::ShaderModule());
		return module;
	}

	ShaderRegistry::ShaderRegistry(const Configuration& config, const std::vector<std::filesystem::path>& shaderPaths) : shaderDir(config.shaderDir)
	{
		if (shaderPaths.size() == 0) {
			bulkLoadShaders(System::IO::getFilesFromDirectory(shaderDir, spirVExtension));
		}
		else {
			bulkLoadShaders(shaderPaths);
		}
		
	}

	ShaderRegistry::~ShaderRegistry()
	{

		for (auto& shader : loadedMap) delete shader.second;
	}

	const Shader& ShaderRegistry::loadShader(const std::filesystem::path& path)
	{
		ASSERT(path != std::string());
		auto [iterator, inserted] = loadedMap.emplace(std::make_pair(path.filename().string(), new Shader(shaderDir / path)));

		ASSERT(inserted);

		return *iterator->second;
	}

	void ShaderRegistry::bulkLoadShaders(const std::vector<std::filesystem::path>& shaderPaths)
	{
		for (auto& path : shaderPaths) {
			loadShader(path);
		}
	}

	const vk::ShaderModule& ShaderRegistry::getShaderRaw(const std::filesystem::path& path) const
	{
		return getShader(path).getModule();
	}

	const Shader& ShaderRegistry::getShader(const std::filesystem::path& path) const
	{
		auto mapEntry = loadedMap.find(path.string());

		ASSERT(mapEntry != loadedMap.end());

		return *mapEntry->second;
	}

	const std::filesystem::path ShaderRegistry::spirVExtension = ".spv";
}