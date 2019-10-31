#include "PipelineHandler.hpp"

#include <array>

namespace Hyperion::Rendering {

	vk::Pipeline PipelineHandler::createForwardPipeline(const vk::Device& device)
	{
		const VideoSettings& videoSettings = context.getVideoSettings();
		const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {

		};

		vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
			{},//vk::PipelineCreateFlagBits::eAllowDerivatives,
			shaderStages.size(),
			shaderStages.data(),
			vk::PipelineVertexInputStateCreateInfo{
				{}
			},
			vk::PipelineInputAssemblyStateCreateInfo{
				{},
				vk::PrimitiveTopology::eTriangleList,
				VK_FALSE
			},
			vk::PipelineTessellationStateCreateInfo{},
			vk::PipelineViewportStateCreateInfo{
				{},
				1,
				vk::Viewport{0, 0, videoSettings.resolution.width, videoSettings.resolution.height, 0, 1},
				1,
				vk::Rect2D{{0, 0}, videoSettings.resolution}
			},
			//TODO Better abstraction over parameters
			vk::PipelineRasterizationStateCreateInfo{
				{},
				VK_FALSE,
				VK_TRUE,
				vk::PolygonMode::eFill,
				vk::CullModeFlagBits::eBack,
				vk::FrontFace::eClockwise,
				VK_FALSE,
			},
			vk::PipelineMultisampleStateCreateInfo{
				{},
				vk::SampleCountFlagBits::e1,
			},
			vk::PipelineDepthStencilStateCreateInfo{ {}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess, VK_FALSE, {}, {}, 0 , 1},
			vk::PipelineColorBlendStateCreateInfo{
				{},
				VK_TRUE,
				vk::LogicOp::eNoOp,
				1,
				vk::PipelineColorBlendAttachmentState{
					VK_TRUE,
					vk::BlendFactor::eSrc1Alpha,
					vk::BlendFactor::eOneMinusSrc1Alpha,
					vk::BlendOp::eAdd,
					vk::BlendFactor::eOne,
					vk::BlendFactor::eZero,
					vk::BlendOp::eAdd,
					vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
				},
				{1, 1, 1, 1}
			},

		};
		
		return device.createGraphicsPipeline(pipelineCache, graphicsPipelineInfo);
	}
	PipelineHandler::PipelineHandler(RenderContext& context) : context(context)
	{
		const vk::Device& device = context.getDevice();

		vk::PipelineCacheCreateInfo cacheCreateInfo{};
		pipelineCache = device.createPipelineCache(cacheCreateInfo);

		forwardPath = createForwardPipeline(device);
	}
	PipelineHandler::~PipelineHandler()
	{
		const vk::Device device = context.getDevice();

		device.destroyPipeline(forwardPath);
		device.destroyPipelineCache(pipelineCache);
	}
}