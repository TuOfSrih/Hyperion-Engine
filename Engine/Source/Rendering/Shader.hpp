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
		declMoveOnly(Shader);
		~Shader();

		const vk::ShaderModule& getModule() const;

	private:
		vk::ShaderModule module;
	};

	class ShaderRegistry {
	public:
		ShaderRegistry() = default;
		ShaderRegistry(const Configuration& config, const std::vector<std::filesystem::path>& shaderNames = {});

		const Shader& loadShader(const std::filesystem::path& path);
		void bulkLoadShaders(const std::vector<std::filesystem::path>& shaderNames);

		const vk::ShaderModule& getShaderRaw(const std::filesystem::path& path) const;
		const Shader& getShader(const std::filesystem::path& path) const;

	private:
		std::unordered_map<std::string, Shader> loadedMap;
		std::string shaderDir;
	};
}