
#include "DrawController.hpp"

#include "Camera.hpp"
#include "RenderContext.hpp"

#include <cassert>


namespace Hyperion::Rendering{
	
	DrawController::DrawController(const PipelineHandler& pipelineHandler) : pipelineHandler(&pipelineHandler)
	{
		const vk::Extent2D& resolution = RenderContext::active->getVideoSettings().resolution;
		registerDefaultRenderTarget(defaultRenderTargetName, new RenderTarget(resolution));
		registerDefaultDepthBuffer(defaultDepthBufferName, new DepthBuffer(resolution));
	}

	DrawController::~DrawController()
	{
		for (auto ptr : textures) delete ptr.second;
		for (auto ptr : renderTargets) delete ptr.second;
		for (auto ptr : depthBuffers) delete ptr.second;
		for (auto ptr : drawObjects) delete ptr;
	}

	void DrawController::registerTexture(const std::string& name, const Texture* texture)
	{
		textures.insert(std::make_pair(name, texture));
	}

	void DrawController::registerRenderTarget(const std::string& name, const RenderTarget* renderTarget)
	{
		renderTargets.insert(std::make_pair(name, renderTarget));
	}

	void DrawController::registerDepthBuffer(const std::string& name, const DepthBuffer* depthBuffer)
	{
		depthBuffers.insert(std::make_pair(name, depthBuffer));
	}

	void DrawController::registerDefaultRenderTarget(const std::string& name, RenderTarget* renderTarget)
	{
		registerRenderTarget(name, renderTarget);
		defaultRenderTarget = renderTarget;
	}

	void DrawController::registerDefaultDepthBuffer(const std::string& name, const DepthBuffer* depthBuffer)
	{
		registerDepthBuffer(name, depthBuffer);
		defaultDepthBuffer = depthBuffer;
	}

	const Texture* DrawController::getTexture(const std::string& name) const
	{
		assert(textures.count(name) > 0);
		return textures.find(name)->second;
	}

	const RenderTarget* DrawController::getRenderTarget(const std::string& name) const
	{
		assert(renderTargets.count(name) > 0);
		return renderTargets.find(name)->second;
	}

	const DepthBuffer* DrawController::getDepthBuffer(const std::string& name) const
	{
		assert(depthBuffers.count(name) > 0);
		return depthBuffers.find(name)->second;
	}

	std::vector<vk::CommandBuffer> DrawController::drawAll() const
	{
		const vk::Device& device = RenderContext::active->getDevice();

		auto cmdBufferVector = device.allocateCommandBuffers(
			{ RenderContext::active->getActiveGraphicsPool(),
			vk::CommandBufferLevel::ePrimary,
			1
			}
		);//TODO Do not use one time command buffers
		const vk::CommandBuffer& cmdBuffer = cmdBufferVector[0];

		startCommandBuffer(cmdBuffer);
		startRenderPass(cmdBuffer, pipelineHandler->getDefaultForward());

		setConstants(cmdBuffer);

		for (auto obj : drawObjects) {
			obj->record(cmdBuffer);
		}

		cmdBuffer.endRenderPass();

		cmdBuffer.end();

		return { cmdBuffer };
	}

	void DrawController::startCommandBuffer(const vk::CommandBuffer& cmdBuffer) const
	{
		cmdBuffer.begin(
			{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr }//TODO use Inheritance
		);
	}

	void DrawController::startRenderPass(const vk::CommandBuffer& cmdBuffer, const Pipeline& pipeline) const
	{
		std::array<vk::ClearValue, 2> clears = { Pipeline::colorClearValue, Pipeline::depthClearValue };
		cmdBuffer.beginRenderPass(
			vk::RenderPassBeginInfo{
				pipeline.getRenderPass(),
				pipeline.getActiveFrameBuffer(),
				{{0, 0}, RenderContext::active->getVideoSettings().resolution},
				static_cast<uint32_t>(clears.size()),
				clears.data()
			},
			vk::SubpassContents::eInline
		);
	}

	void DrawController::setConstants(const vk::CommandBuffer& cmdBuffer) const
	{
		const Pipeline& pipeline = pipelineHandler->getDefaultForward();
		const VideoSettings& videoSettings = RenderContext::active->getVideoSettings();
		

		cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getRaw());

		cmdBuffer.setViewport(0, {{0, 0, static_cast<float>(videoSettings.resolution.width), static_cast<float>(videoSettings.resolution.height), DepthBuffer::minDepth, DepthBuffer::maxDepth} });

		cmdBuffer.setScissor(0, { {{0, 0}, videoSettings.resolution} });
	}

	const std::string DrawController::defaultRenderTargetName = "defaultRenderTarget";
	const std::string DrawController::defaultDepthBufferName = "defaultDepthBuffer";

	void VisualEntity::record(const vk::CommandBuffer& commandBuffer) const
	{
		uint32_t activeIndex = RenderContext::active->getActiveBufferIndex();
		UBO ubo = { transform.getModelMatrix(), Camera::getActive().getViewMatrix(), Camera::getActive().getProjectionMatrix() };
		void* dest = ubos.at(activeIndex).mapMemory();
		std::memcpy(dest, &ubo, sizeof(ubo));
		ubos.at(RenderContext::active->getActiveBufferIndex()).unmapMemory();

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getLayout(), 0, { getActiveDescriptorSet() }, {});

		mesh->bind(commandBuffer);
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getLayout(), 0, { getActiveDescriptorSet() }, {});
		mesh->draw(commandBuffer);
	}

	void VisualEntity::allocateDescriptorSet()
	{

		const vk::Device& device = RenderContext::active->getDevice();
		
		//TODO make Universally work
		std::vector<vk::DescriptorSetLayout> layouts(RenderContext::active->getVideoSettings().bufferImageCount, pipeline->getDescriptorSetLayouts().at(0));
		MVPDescriptors = device.allocateDescriptorSets(
			{
				pipeline->getDescriptorPool(),
				static_cast<uint32_t>(layouts.size()),
				layouts.data()
			}
		);

		for (uint32_t i = 0; i < RenderContext::active->getVideoSettings().bufferImageCount; ++i) {
			
			vk::DescriptorBufferInfo bufferInfo = {
							ubos.at(i).getHandle(),
							0,
							sizeof(System::Memory::UniformBuffer)
			};
			device.updateDescriptorSets(
				{
					vk::WriteDescriptorSet{
						MVPDescriptors.at(i),
						0,
						0,
						1, 
						vk::DescriptorType::eUniformBuffer,
						nullptr,
						&bufferInfo,
						nullptr
					}
				},
				{}
			);
		}
		
	}
	const vk::DescriptorSet& VisualEntity::getActiveDescriptorSet() const
	{
		return MVPDescriptors.at(RenderContext::active->getActiveBufferIndex());
	}
}