#include "Pipeline.hpp"

#include <array>

namespace Hyperion::Rendering {

	PipelineHandler::PipelineHandler(RenderContext& context) : context(context)
	{
		const vk::Device& device = context.getDevice();

		vk::PipelineCacheCreateInfo cacheCreateInfo{};
		pipelineCache = device.createPipelineCache(cacheCreateInfo);
		
		forwardPath = Pipeline{ context, *this };
	}
	PipelineHandler::~PipelineHandler()
	{
		const vk::Device device = context.getDevice();

		//device.destroyPipeline(forwardPath);
		device.destroyPipelineCache(pipelineCache);
	}
	vk::PipelineCache& PipelineHandler::getPipelineCache()
	{
		return pipelineCache;
	}
	//TODO Should not exist
	Pipeline::Pipeline()
	{
	}
	Pipeline::Pipeline(RenderContext& context, PipelineHandler& pipelineHandler)
	{
		const VideoSettings& videoSettings = context.getVideoSettings();
		const vk::Device device = context.getDevice();
		const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
			
		};

		const vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		
		const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
				{},
				vk::PrimitiveTopology::eTriangleList,
				VK_FALSE
		};

		const vk::PipelineTessellationStateCreateInfo tesselationInfo{};

		const vk::Viewport viewport{ 0, 0, static_cast<float>(videoSettings.resolution.width), 
			static_cast<float>(videoSettings.resolution.height), 0, 1 };
		const vk::Rect2D scissor{ {0, 0}, videoSettings.resolution };

		const vk::PipelineViewportStateCreateInfo viewportInfo{
				{},
				1,
				&viewport,
				1,
				&scissor
		};

		//TODO Better abstraction over parameters
		const vk::PipelineRasterizationStateCreateInfo rasterizationInfo{
			{},
			VK_FALSE,
			VK_TRUE,
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack,
			vk::FrontFace::eClockwise,
			VK_FALSE,
		};

		const vk::PipelineMultisampleStateCreateInfo multisampleInfo{
			{},
			vk::SampleCountFlagBits::e1,
		};

		const vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{ {}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess, 
			VK_FALSE, VK_FALSE, {}, {}, 0, 1 };

		const vk::PipelineColorBlendAttachmentState blendAttachmentState{
				VK_TRUE,
				vk::BlendFactor::eSrc1Alpha,
				vk::BlendFactor::eOneMinusSrc1Alpha,
				vk::BlendOp::eAdd,
				vk::BlendFactor::eOne,
				vk::BlendFactor::eZero,
				vk::BlendOp::eAdd,
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		};

		const vk::PipelineColorBlendStateCreateInfo blendInfo{
			{},
			VK_TRUE,
			vk::LogicOp::eNoOp,
			1,
			&blendAttachmentState,
			{1, 1, 1, 1}
		};

		const std::array<vk::DynamicState, 2>  dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};

		const vk::PipelineDynamicStateCreateInfo dynamicStatesInfo{
			{}, dynamicStates.size(), dynamicStates.data()
		};

		vk::PipelineLayoutCreateInfo{

		}

		vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
			{},//vk::PipelineCreateFlagBits::eAllowDerivatives,
			static_cast<uint32_t>(shaderStages.size()),
			shaderStages.data(),
			&vertexInputInfo,
			&inputAssemblyInfo,
			&tesselationInfo,
			&viewportInfo,
			&rasterizationInfo,
			&multisampleInfo,
			&depthStencilInfo,
			&blendInfo,
			&dynamicStatesInfo,
		};

		pipeline = context.getDevice().createGraphicsPipeline(pipelineHandler.getPipelineCache(), graphicsPipelineInfo);
	}
}