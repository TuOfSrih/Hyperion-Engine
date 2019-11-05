#pragma once

#include "vulkan/vulkan.hpp"

#include "Cpp/AdvancedOperators.hpp"

namespace Hyperion::Rendering {

	//TODO Remove this weird definition
	class PipelineHandler;
	class Pipeline{

		PipelineHandler* pipelineHandler = nullptr;
		vk::Pipeline pipeline;
		vk::RenderPass renderpass;

		vk::RenderPass getRenderPass();
		vk::PipelineVertexInputStateCreateInfo getVertexInputInfo();
		vk::PipelineInputAssemblyStateCreateInfo getInputAssemblyInfo();
		vk::PipelineTessellationStateCreateInfo getTesselationInfo();
		vk::PipelineViewportStateCreateInfo getViewportInfo();
		vk::PipelineRasterizationStateCreateInfo getRasterizationInfo();
		vk::PipelineMultisampleStateCreateInfo getMultiSampleInfo();
		vk::PipelineDepthStencilStateCreateInfo getDepthStencilInfo();
		vk::PipelineColorBlendStateCreateInfo getBlendInfo();
		vk::PipelineDynamicStateCreateInfo getDynamicStateInfo();
		vk::PipelineLayout getPipelineLayout();
		
	public:
		Pipeline();
		Pipeline(PipelineHandler* pipelineHandler);

	};

	class PipelineHandler{

		vk::PipelineCache pipelineCache;
		Pipeline forwardPath;

	public:

		PipelineHandler();
		~PipelineHandler();

		vk::PipelineCache& getPipelineCache();
		
	};
}