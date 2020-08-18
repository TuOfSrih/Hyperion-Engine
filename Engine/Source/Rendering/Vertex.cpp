
#include "Vertex.hpp"

#include "RenderContext.hpp"

namespace Hyperion::Rendering {
	
	std::vector<vk::VertexInputAttributeDescription> CombinedVertex::getInputAttributeDescriptions () const
	{
		std::vector<vk::VertexInputAttributeDescription> result;
		result.reserve(4);

		result.emplace_back(vk::VertexInputAttributeDescription{
			0,
			0,
			VertexType::vec3Format,
			offsetof(CombinedVertex, pos)
		});

		result.emplace_back(vk::VertexInputAttributeDescription{
			1,
			0,
			VertexType::vec3Format,
			offsetof(CombinedVertex, normals)
		});

		result.emplace_back(vk::VertexInputAttributeDescription{
			2,
			0,
			VertexType::vec3Format,
			offsetof(CombinedVertex, color)
		});

		result.emplace_back(vk::VertexInputAttributeDescription{
			3,
			0,
			VertexType::vec2Format,
			offsetof(CombinedVertex, pos)
		});
		return result;
	}
	
	vk::VertexInputBindingDescription CombinedVertex::getInputBindingDescription() const
	{
		return {
			0,
			sizeof(CombinedVertex),
			vk::VertexInputRate::eVertex
		};
	}
	vk::DescriptorSetLayout CombinedVertex::getDescriptorLayout() const
	{
		const vk::Device& device = RenderContext::active->getDevice();

		vk::DescriptorSetLayoutBinding descriptorBinding{
			0,
			vk::DescriptorType::eUniformBuffer,
			1,
			vk::ShaderStageFlagBits::eVertex,
			nullptr
		};

		return device.createDescriptorSetLayout({
			{},
			1,
			&descriptorBinding
		});
	}
	std::vector<vk::VertexInputAttributeDescription> RawVertex::getInputAttributeDescriptions() const
	{
		return { {0, 0, VertexType::vec3Format, offsetof(RawVertex, pos)} };
	}
	vk::VertexInputBindingDescription RawVertex::getInputBindingDescription() const
	{
		return {//Unnecessary?
			0,
			sizeof(RawVertex),
			vk::VertexInputRate::eVertex
		};
	}
	vk::DescriptorSetLayout RawVertex::getDescriptorLayout() const
	{
		const vk::Device& device = RenderContext::active->getDevice();

		//TODO Proper stages
		vk::DescriptorSetLayoutBinding descriptorBinding{
			0,
			vk::DescriptorType::eUniformBuffer,
			1,
			vk::ShaderStageFlagBits::eAll,
			nullptr
		};

		return device.createDescriptorSetLayout({
			{},
			1,
			&descriptorBinding
			});
	}
}