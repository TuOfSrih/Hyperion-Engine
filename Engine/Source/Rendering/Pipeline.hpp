#pragma once

#include "vulkan/vulkan.hpp"

#include "RenderContext.hpp"

namespace Hyperion::Rendering {

	//TODO Remove this weird definition
	class RenderContext;
	class PipelineHandler;
	class Pipeline{

		vk::Pipeline pipeline;
		vk::RenderPass renderpass;
	public:
		Pipeline();
		Pipeline(RenderContext& context, PipelineHandler& pipelineHandler);

	};

	class PipelineHandler{

		RenderContext& context;
		vk::PipelineCache pipelineCache;
		Pipeline forwardPath;

		//vk::Pipeline createForwardPipeline(const vk::Device& device);
	public:

		PipelineHandler(RenderContext& context);
		~PipelineHandler();

		vk::PipelineCache& getPipelineCache();
		
	};
}