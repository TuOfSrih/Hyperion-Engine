
#include "Vertex.hpp"


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
			sizeof(*this),
			vk::VertexInputRate::eVertex
		};
	}
}