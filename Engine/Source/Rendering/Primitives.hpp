#pragma once

#include "Rendering/Mesh.hpp"

namespace Hyperion::Rendering {

	class Primitive {
	public:
		Primitive(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices):
		virtual ~Primitive() = default;
	private:
		Mesh
	};
}