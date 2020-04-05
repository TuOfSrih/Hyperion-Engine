#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "vulkan/vulkan.hpp"

#include "RenderContext.hpp"
#include "System/Memory.hpp"

namespace Hyperion::Rendering {

	class Transform {
	public:
		Transform() = default;
		Transform(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

	private:
		glm::vec3 position;
		glm::quat orientation;
		glm::vec3 scale;
	};

	class VertexOnlyMesh {
	public:
		VertexOnlyMesh(std::vector<glm::vec3> vertices) 
			: vertexBuffer({ vertices.data(), vertices.size() * sizeof(glm::vec3)}){};
		defaultTouch(VertexOnlyMesh);

		virtual ~VertexOnlyMesh() = default;

	protected:
		System::Memory::VertexBuffer vertexBuffer;
	};

	class Mesh : VertexOnlyMesh{
	public:
		Mesh(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices)
			: indexBuffer(indices.data(), indices.size() * sizeof(uint32_t)), 
			VertexOnlyMesh(vertices) {};



	private:
		System::Memory::IndexBuffer indexBuffer;
	};
	
	enum class PrimitiveType {
		Cube,
		Sphere,
		Cylinder,
		Capsule,
		OneSidedPlane,
		Plane
	};

	const Mesh* GetPrimitive(PrimitiveType type);

	class MeshInstance {
	public:
		MeshInstance(Transform transform, const Mesh* mesh) : transform(transform), mesh(mesh){};
		MeshInstance(Transform transform, PrimitiveType primitive) : MeshInstance(transform, GetPrimitive(primitive)) {};

	private:
		Transform transform;
		const Mesh* mesh;
	};
}