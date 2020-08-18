#pragma once

#include "System/Configuration.hpp"
#include "Cpp/AdvancedOperators.hpp"

#include "vulkan/vulkan.hpp"

#include <filesystem>
#include <unordered_map>


namespace Hyperion::Rendering {

	class Shader {
	public:
		Shader() = default;
		explicit Shader(const std::filesystem::path& path);
		noCopy(Shader);
		declMove(Shader);
		~Shader();

		const vk::ShaderModule& getModule() const;

	private:
		vk::ShaderModule module;
	};

	class ShaderRegistry {
	public:
		ShaderRegistry() = default;
		ShaderRegistry(const Configuration& config, const std::vector<std::filesystem::path>& shaderNames = {});
		noCopy(ShaderRegistry);
		defaultMove(ShaderRegistry);
		~ShaderRegistry();

		const Shader& loadShader(const std::filesystem::path& path);
		void bulkLoadShaders(const std::vector<std::filesystem::path>& shaderNames);

		const vk::ShaderModule& getShaderRaw(const std::filesystem::path& path) const;
		const Shader& getShader(const std::filesystem::path& path) const;

	private:

		static const std::filesystem::path spirVExtension;
		std::unordered_map<std::string, const Shader*> loadedMap;
		std::filesystem::path shaderDir;
	};
}