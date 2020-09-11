
#include "Mesh.hpp"

#include "Transform.hpp"


namespace Hyperion::Rendering {


	const Mesh* GetPrimitive(PrimitiveType type)
	{
		//TODO Add remaining Primitives + Add Cleanup
		switch (type) {

		case PrimitiveType::Cube:
			{
				//Right handed with y upwards looking from the outside
				static Mesh cubeMesh(
					{
						{0, 0, 0},
						{1, 0, 0},
						{0, 0, 1},
						{1, 0, 1},
						{0, 1, 0},
						{1, 1, 0},
						{0, 1, 1},
						{1, 1, 1}
					},
					{
						//Bottom
						0, 2, 1,
						3, 1, 2,

						//Top
						4, 5, 6,
						7, 6, 5,

						//Right
						3, 7, 1,
						5, 1, 7,

						//Left
						2, 0, 6,
						4, 6, 0,

						//Front
						3, 2, 7,
						6, 7, 2,

						//Back
						1, 5, 0,
						4, 0, 5
					}
					);
				return &cubeMesh;
			}
			break;
		case PrimitiveType::Sphere:
			{
				static Mesh sphereMesh(
					{
					},
					{
					}
					);
				return &sphereMesh;
			}
			
		break;
		case PrimitiveType::Cylinder:
			{
				static Mesh cylinderMesh(
					{
					},
					{
					}
					);
				return &cylinderMesh;
			}
		break;
		case PrimitiveType::Capsule:
			{
				static Mesh capsuleMesh(
					{
					},
					{
					}
					);
				return &capsuleMesh;
			}
		break;
		case PrimitiveType::OneSidedPlane:
			{
				static Mesh oneSidedPlaneMesh(
					{
					},
					{
					}
					);
				return &oneSidedPlaneMesh;
			}
			break;
		case PrimitiveType::Plane:
			{
				static Mesh planeMesh(
					{
					},
					{
					}
					);
				return &planeMesh;
			}
			break;
		}
		return nullptr;
	}
	void VertexOnlyMesh::bind(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.bindVertexBuffers(0, { vertexBuffer.getHandle() }, { {0} });
	}

	void VertexOnlyMesh::draw(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.draw(3, 1, 0, 0);//TODO Get from buffer
	}

	void Mesh::bind(const vk::CommandBuffer& commandBuffer) const
	{
		VertexOnlyMesh::bind(commandBuffer);
		commandBuffer.bindIndexBuffer(indexBuffer.getHandle(), 0, System::Memory::IndexBuffer::indexType);
	}
	void Mesh::draw(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.drawIndexed(3, 1, 0, 0, 0);//TODO Get from buffer
	}
}


