#pragma once

#include "vulkan/vulkan.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <vector>


namespace Hyperion::Rendering {


	class VertexType {//TODO Find better solution with less memory consumption
	public:
		VertexType() = default;
		virtual std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions() const = 0;
		virtual vk::VertexInputBindingDescription getInputBindingDescription() const = 0;
		virtual vk::DescriptorSetLayout getDescriptorLayout() const = 0;

		static const vk::Format vec2Format = vk::Format::eR32G32Sfloat;
		static const vk::Format vec3Format = vk::Format::eR32G32B32Sfloat;

	};

	class RawVertex : public VertexType {
	public:
		virtual std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions() const override;
		virtual vk::VertexInputBindingDescription getInputBindingDescription() const override;
		virtual vk::DescriptorSetLayout getDescriptorLayout() const override;

	private:
		glm::vec3 pos;
	};

	class OrientedVertex : public VertexType {

	};

	class ColoredVertex : public VertexType {

	};

	class UVMappedVertex : public VertexType {

	};

	class CombinedVertex : public VertexType {
	public:
		CombinedVertex() = default;
		virtual std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions() const override;
		virtual vk::VertexInputBindingDescription getInputBindingDescription() const override;
		virtual vk::DescriptorSetLayout getDescriptorLayout() const override;

	private:
		glm::vec3 pos;
		glm::vec3 normals;
		glm::vec3 color;
		glm::vec2 uv;
	};
}