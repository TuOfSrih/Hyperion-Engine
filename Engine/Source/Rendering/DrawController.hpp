#pragma once

#include "Image2D.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"

#include "System/Memory.hpp"

#include <unordered_map>


namespace Hyperion::Rendering {

	

	struct UBO {
		glm::mat4x4 modelMatrix;
		glm::mat4x4 viewMatrix;
		glm::mat4x4 projMatrix;
	};

	class VisualEntity {
	public:
		VisualEntity( Transform transform, const Mesh* mesh, const Pipeline* pipeline) : pipeline(pipeline), transform(transform), mesh(mesh){};
		VisualEntity( Transform transform, PrimitiveType primitive, const Pipeline* pipeline ) : pipeline(pipeline), transform(transform), mesh(GetPrimitive(primitive)){};


		virtual void record(const vk::CommandBuffer& commandBuffer) const;

	private:
		void allocateDescriptorSet();
		vk::DescriptorSet getActiveDescriptorSet() const;

		std::vector<vk::DescriptorSet> MVPDescriptors;

		const Pipeline* pipeline = nullptr;//Check if unnecessary TODO
		static_assert(VideoSettings::maxBufferImageCount == 3);
		std::array<System::Memory::UniformBuffer, VideoSettings::maxBufferImageCount> ubos = {
			System::Memory::UniformBuffer(sizeof(UBO)),
			System::Memory::UniformBuffer(sizeof(UBO)),
			System::Memory::UniformBuffer(sizeof(UBO))
		};
		Transform transform;
		const Mesh* mesh;
	};

	class DrawController {
	public:
		DrawController() = default;
		defaultTouch(DrawController);
		~DrawController();

		void registerTexture(const std::string& name, const Texture* texture);
		void registerRenderTarget(const std::string& name, const RenderTarget* renderTarget);
		void registerDepthBuffer(const std::string& name, const DepthBuffer* depthBuffer);

		void registerDefaultRenderTarget(const std::string& name, const RenderTarget* renderTarget);
		void registerDefaultDepthBuffer(const std::string& name, const DepthBuffer* depthBuffer);

		std::vector<vk::CommandBuffer> drawAll() const;

	private:

		void setConstants(const vk::CommandBuffer& cmdBuffer) const;

		const PipelineHandler* pipelineHandler;

		std::unordered_map<std::string, const Texture*> textures;
		std::unordered_map<std::string, const RenderTarget*> renderTargets;
		std::unordered_map<std::string, const DepthBuffer*> depthBuffers;

		const RenderTarget* defaultRenderTarget = nullptr;
		const DepthBuffer* defaultDepthBuffer = nullptr;

		std::vector<const VisualEntity*> drawObjects;

	};
}