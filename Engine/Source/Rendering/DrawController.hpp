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
		VisualEntity(Transform transform, const Mesh* mesh, const Pipeline* pipeline) : pipeline(pipeline), transform(transform), mesh(mesh) { allocateDescriptorSet(); };
		VisualEntity( Transform transform, PrimitiveType primitive, const Pipeline* pipeline ) : pipeline(pipeline), transform(transform), mesh(GetPrimitive(primitive)){ allocateDescriptorSet(); };

		virtual void record(const vk::CommandBuffer& commandBuffer) const;
		//TODO add virtual destructor
		void update();

	private:

		const vk::DescriptorSet& getActiveDescriptorSet() const;
		void allocateDescriptorSet();


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
		DrawController(const PipelineHandler& pipelineHandler);
		defaultTouch(DrawController);
		~DrawController();

		//Remove
		static const std::string defaultRenderTargetName;
		static const std::string defaultDepthBufferName;

		void registerDrawObject(const VisualEntity* object);

		void registerTexture(const std::string& name, const Texture* texture);
		void registerRenderTarget(const std::string& name, const RenderTarget* renderTarget);
		void registerDepthBuffer(const std::string& name, const DepthBuffer* depthBuffer);

		void registerDefaultRenderTarget(const std::string& name, RenderTarget* renderTarget);
		void registerDefaultDepthBuffer(const std::string& name, const DepthBuffer* depthBuffer);

		const Texture* getTexture(const std::string& name) const;
		const RenderTarget* getRenderTarget(const std::string& name) const;
		const DepthBuffer* getDepthBuffer(const std::string& name) const;

		std::vector<vk::CommandBuffer> drawAll() const;

	private:

		void startCommandBuffer(const vk::CommandBuffer& cmdBuffer) const;
		void startRenderPass(const vk::CommandBuffer& cmdBuffer, const Pipeline& pipeline) const;
		void setConstants(const vk::CommandBuffer& cmdBuffer) const;

		const PipelineHandler* pipelineHandler = nullptr;

		std::unordered_map<std::string, const Texture*> textures;
		std::unordered_map<std::string, const RenderTarget*> renderTargets;
		std::unordered_map<std::string, const DepthBuffer*> depthBuffers;

		//TODO possibly const again
		RenderTarget* defaultRenderTarget = nullptr;
		const DepthBuffer* defaultDepthBuffer = nullptr;

		std::vector<const VisualEntity*> drawObjects;

	};
}