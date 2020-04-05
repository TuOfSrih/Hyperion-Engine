#include "Pipeline.hpp"

#include <array>

#include "RenderContext.hpp"

namespace Hyperion::Rendering {
	//TODO Should not exist
	Pipeline::Pipeline()
	{
	}
	Pipeline::Pipeline(PipelineHandler* pipelineHandler): pipelineHandler(pipelineHandler)
	{
		const VideoSettings& videoSettings = RenderContext::active->getVideoSettings();
		videoSettings.bufferImageCount;
		const vk::Device device = RenderContext::active->getDevice();
		renderpass = getRenderPass();
		const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderInfo();
		const vk::PipelineVertexInputStateCreateInfo vertexInputInfo = getVertexInputInfo();
		const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = getInputAssemblyInfo();
		const vk::PipelineTessellationStateCreateInfo tesselationInfo = getTesselationInfo();
		const vk::PipelineViewportStateCreateInfo viewportInfo = getViewportInfo();
		//TODO Better abstraction over parameters
		const vk::PipelineRasterizationStateCreateInfo rasterizationInfo = getRasterizationInfo();
		const vk::PipelineMultisampleStateCreateInfo multisampleInfo = getMultiSampleInfo();
		const vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = getDepthStencilInfo();
		const vk::PipelineColorBlendStateCreateInfo blendInfo = getBlendInfo();
		const vk::PipelineDynamicStateCreateInfo dynamicStatesInfo = getDynamicStateInfo();
		const vk::PipelineLayout pipelineLayout = getPipelineLayout();


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
			pipelineLayout,
			renderpass,
			0,
			pipeline,
			0

			//TODO Pipeline derivatives
		};

		pipeline = RenderContext::active->getDevice().createGraphicsPipeline(pipelineHandler->getPipelineCache(), graphicsPipelineInfo);
	}

	vk::PipelineVertexInputStateCreateInfo Pipeline::getVertexInputInfo() {

		return vk::PipelineVertexInputStateCreateInfo{};
	}

	vk::PipelineInputAssemblyStateCreateInfo Pipeline::getInputAssemblyInfo() {

		return vk::PipelineInputAssemblyStateCreateInfo{
				{},
				vk::PrimitiveTopology::eTriangleList,
				VK_FALSE
		};
	}
	
	vk::PipelineTessellationStateCreateInfo Pipeline::getTesselationInfo () {

		return vk::PipelineTessellationStateCreateInfo {

		};
	}

	vk::PipelineViewportStateCreateInfo Pipeline::getViewportInfo() {

		const VideoSettings& videoSettings = RenderContext::active->getVideoSettings();

		const vk::Viewport viewport{ 
			0, 
			0, 
			static_cast<float>(videoSettings.resolution.width),
			static_cast<float>(videoSettings.resolution.height), 
			0, 
			1 
		};

		const vk::Rect2D scissor{ {0, 0}, videoSettings.resolution };

		return vk::PipelineViewportStateCreateInfo{
				{},
				1,
				&viewport,
				1,
				&scissor
		};
	}

	vk::PipelineRasterizationStateCreateInfo Pipeline::getRasterizationInfo() {

		return vk::PipelineRasterizationStateCreateInfo{
			{},
			VK_FALSE,
			VK_TRUE,
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack,
			vk::FrontFace::eClockwise,
			VK_FALSE,
		};
	}

	vk::PipelineMultisampleStateCreateInfo Pipeline::getMultiSampleInfo() {

		return vk::PipelineMultisampleStateCreateInfo{
			{},
			vk::SampleCountFlagBits::e1,
		};
	}

	vk::PipelineDepthStencilStateCreateInfo Pipeline::getDepthStencilInfo() {

		return vk::PipelineDepthStencilStateCreateInfo{ 
			{},
			VK_TRUE,
			VK_TRUE,
			vk::CompareOp::eLess,
			VK_FALSE,
			VK_FALSE,
			{},
			{},
			0,
			1
		};
	}

	vk::PipelineColorBlendStateCreateInfo Pipeline::getBlendInfo() {

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

		return vk::PipelineColorBlendStateCreateInfo{
			{},
			VK_TRUE,
			vk::LogicOp::eNoOp,
			1,
			&blendAttachmentState,
			{1, 1, 1, 1}
		};
	}

	vk::PipelineDynamicStateCreateInfo Pipeline::getDynamicStateInfo() {

		const std::array<vk::DynamicState, 2>  dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		return vk::PipelineDynamicStateCreateInfo{
			{}, static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data()
		};
	}

	vk::PipelineLayout Pipeline::getPipelineLayout()
	{
		const vk::Device device = RenderContext::active->getDevice();

		const vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
			//{}, //TODO Check whether this is correct

		};
		
		vk::PipelineLayoutCreateInfo layoutInfo{

		};
		return device.createPipelineLayout(layoutInfo);
	}

	vk::RenderPass Pipeline::getRenderPass()
	{
		vk::RenderPassCreateInfo renderPassInfo{
			//{},
			//static_cast<uint32_t>(attachmentDescriptions.size()),attachmentDescriptions.data(),

		};

		return RenderContext::active->getDevice().createRenderPass(renderPassInfo);
	}

	std::vector<vk::PipelineShaderStageCreateInfo> Pipeline::getShaderInfo()
	{
		return std::vector<vk::PipelineShaderStageCreateInfo>();
	}

	PipelineHandler::PipelineHandler(const vk::Device& device)
	{

		vk::PipelineCacheCreateInfo cacheCreateInfo{};
		pipelineCache = device.createPipelineCache(cacheCreateInfo);

		forwardPath = Pipeline{ };
	}
	PipelineHandler::~PipelineHandler()
	{
		const vk::Device device = RenderContext::active->getDevice();

		device.destroyPipelineCache(pipelineCache);
	}
	vk::PipelineCache& PipelineHandler::getPipelineCache()
	{
		return pipelineCache;
	}

}