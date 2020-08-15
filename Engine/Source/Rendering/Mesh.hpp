#pragma once

#include "System/Memory.hpp"

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"

#include <vector>


namespace Hyperion::Rendering {

	class VertexOnlyMesh {
	public:
		VertexOnlyMesh(std::vector<glm::vec3> vertices) 
			: vertexBuffer({ vertices.data(), vertices.size() * sizeof(glm::vec3)}){};
		defaultTouch(VertexOnlyMesh);

		virtual ~VertexOnlyMesh() = default;

		virtual void bind(const vk::CommandBuffer& commandBuffer) const;
		virtual void draw(const vk::CommandBuffer& commandBuffer) const;

	protected:
		System::Memory::VertexBuffer vertexBuffer;
	};

	class Mesh : VertexOnlyMesh{
	public:
		Mesh(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices)
			: indexBuffer(indices.data(), indices.size() * sizeof(uint32_t)), 
			VertexOnlyMesh(vertices) {};

		virtual void bind(const vk::CommandBuffer& commandBuffer) const override;
		virtual void draw(const vk::CommandBuffer& commandBuffer) const override;

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

	/*class MeshInstance {
	public:
		MeshInstance(Transform transform, const Mesh* mesh) : transform(transform), mesh(mesh){};
		MeshInstance(Transform transform, PrimitiveType primitive) : MeshInstance(transform, GetPrimitive(primitive)) {};

	private:
		Transform transform;
		const Mesh* mesh;
	};*/
}