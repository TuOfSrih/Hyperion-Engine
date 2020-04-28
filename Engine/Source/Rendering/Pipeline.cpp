
#include "Pipeline.hpp"

#include "RenderContext.hpp"

#include <array>

namespace Hyperion::Rendering {
	//TODO Should not exist
	Pipeline::Pipeline()
	{
	}
	Pipeline::Pipeline(const PipelineHandler& pipelineHandler, const PipelineInfo& pipelineInfo): pipelineHandler(&pipelineHandler)
	{
		const VideoSettings& videoSettings = RenderContext::active->getVideoSettings();
		const vk::Device device = RenderContext::active->getDevice();
		renderpass = getRenderPass(pipelineInfo);
		const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderInfo(pipelineInfo);
		const vk::PipelineVertexInputStateCreateInfo vertexInputInfo = getVertexInputInfo(pipelineInfo);
		const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = getInputAssemblyInfo(pipelineInfo);
		const vk::PipelineTessellationStateCreateInfo tesselationInfo = getTesselationInfo(pipelineInfo);
		const vk::PipelineViewportStateCreateInfo viewportInfo = getViewportInfo(pipelineInfo);
		//TODO Better abstraction over parameters
		const vk::PipelineRasterizationStateCreateInfo rasterizationInfo = getRasterizationInfo(pipelineInfo);
		const vk::PipelineMultisampleStateCreateInfo multisampleInfo = getMultiSampleInfo(pipelineInfo);
		const vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = getDepthStencilInfo(pipelineInfo);
		const vk::PipelineColorBlendStateCreateInfo blendInfo = getBlendInfo(pipelineInfo);
		const vk::PipelineDynamicStateCreateInfo dynamicStatesInfo = getDynamicStateInfo(pipelineInfo);
		const vk::PipelineLayout pipelineLayout = getPipelineLayout(pipelineInfo);


		vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
			{},//vk::PipelineCreateFlagBits::eAllowDerivatives, TODO use derivatives
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

		pipeline = RenderContext::active->getDevice().createGraphicsPipeline(pipelineHandler.getPipelineCache(), graphicsPipelineInfo);
	}

	const vk::PipelineVertexInputStateCreateInfo Pipeline::getVertexInputInfo(const PipelineInfo& pipelineInfo) {

		//Dont return reference to local variable
		auto attributeDescriptions = pipelineInfo.vertexType->getInputAttributeDescriptions();
		auto bindingDescription = pipelineInfo.vertexType->getInputBindingDescription();
		return vk::PipelineVertexInputStateCreateInfo{
			vk::PipelineVertexInputStateCreateFlags{},
			1,
			&bindingDescription,
			attributeDescriptions.size(),
			attributeDescriptions.data()
		};
	}

	const vk::PipelineInputAssemblyStateCreateInfo Pipeline::getInputAssemblyInfo(const PipelineInfo& pipelineInfo) {

		return vk::PipelineInputAssemblyStateCreateInfo{
				{},
				pipelineInfo.topology,
				VK_FALSE//Revisit later
		};
	}
	
	const vk::PipelineTessellationStateCreateInfo Pipeline::getTesselationInfo (const PipelineInfo& pipelineInfo) {

		return vk::PipelineTessellationStateCreateInfo {
			{},
			pipelineInfo.tesselationPatchControlPoints
		};
	}

	const vk::PipelineViewportStateCreateInfo Pipeline::getViewportInfo(const PipelineInfo& pipelineInfo) {

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

	const vk::PipelineRasterizationStateCreateInfo Pipeline::getRasterizationInfo(const PipelineInfo& pipelineInfo) {

		bool isLine = pipelineInfo.topology == vk::PrimitiveTopology::eLineList ||
			pipelineInfo.topology == vk::PrimitiveTopology::eLineListWithAdjacency ||
			pipelineInfo.topology == vk::PrimitiveTopology::eLineStrip;

		bool isTriangle = pipelineInfo.topology == vk::PrimitiveTopology::eTriangleFan ||
			pipelineInfo.topology == vk::PrimitiveTopology::eTriangleList ||
			pipelineInfo.topology == vk::PrimitiveTopology::eTriangleListWithAdjacency ||
			pipelineInfo.topology == vk::PrimitiveTopology::eTriangleStrip ||
			pipelineInfo.topology == vk::PrimitiveTopology::eTriangleStripWithAdjacency;

		bool isPoint = pipelineInfo.topology == vk::PrimitiveTopology::ePointList;

		return vk::PipelineRasterizationStateCreateInfo{
			{},
			VK_FALSE,//REVISIT
			VK_FALSE,
			(isTriangle || isPoint ? vk::PolygonMode::eFill :vk::PolygonMode::eLine),
			pipelineInfo.cullingFlags,
			RenderOrder,
			VK_FALSE,
		};
	}

	const vk::PipelineMultisampleStateCreateInfo Pipeline::getMultiSampleInfo(const PipelineInfo& pipelineInfo) {

		return vk::PipelineMultisampleStateCreateInfo{
			{},
			pipelineInfo.sampleAmount,
		};
	}

	const vk::PipelineDepthStencilStateCreateInfo Pipeline::getDepthStencilInfo(const PipelineInfo& pipelineInfo) {

		return vk::PipelineDepthStencilStateCreateInfo{ 
			{},
			pipelineInfo.flags & DepthStencilFlagBits::depthTestEnable,
			pipelineInfo.flags & DepthStencilFlagBits::depthWriteEnable,
			pipelineInfo.depthCompareOp,
			VK_FALSE,
			pipelineInfo.flags & DepthStencilFlagBits::stencilTestEnable,
			{pipelineInfo.stencilFailOp, pipelineInfo.stencilPassOp, pipelineInfo.stencilDepthFailOp, pipelineInfo.stencilCompareOp, 0xFFFFFFFF, 0xFFFFFFFF, pipelineInfo.stencilReference},
			{pipelineInfo.stencilFailOp, pipelineInfo.stencilPassOp, pipelineInfo.stencilDepthFailOp, pipelineInfo.stencilCompareOp, 0xFFFFFFFF, 0xFFFFFFFF, pipelineInfo.stencilReference},
			0.0f,
			1.0f
		};
	}

	const vk::PipelineColorBlendStateCreateInfo Pipeline::getBlendInfo(const PipelineInfo& pipelineInfo) {


		if (pipelineInfo.blendMode == BlendMode::alphaBlend) {
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
				VK_FALSE,
				vk::LogicOp::eNoOp,
				1,
				&blendAttachmentState,
				{1, 1, 1, 1}
			};
		}

		Debug::missingFunctionality("No blend mode found");
		
	}

	const vk::PipelineDynamicStateCreateInfo Pipeline::getDynamicStateInfo(const PipelineInfo& pipelineInfo) {

		const std::array<vk::DynamicState, 2>  dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		return vk::PipelineDynamicStateCreateInfo{
			{}, static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data()
		};
	}

	const vk::PipelineLayout Pipeline::getPipelineLayout(const PipelineInfo& pipelineInfo)
	{
		const vk::Device device = RenderContext::active->getDevice();

		std::vector<vk::DescriptorSetLayout> descriptorLayoutInfos;
		descriptorLayoutInfos.reserve(2);
		pipelineInfo.vertexType->getDescriptorLayout();

		for (auto& inputAtt : pipelineInfo.inputAttachments) descriptorLayoutInfos.emplace_back(inputAtt->getDescriptorSetLayout());
		
		vk::PipelineLayoutCreateInfo layoutInfo{
			{},
			descriptorLayoutInfos.size(),
			descriptorLayoutInfos.data(),
			0, 
			nullptr
		};
		return device.createPipelineLayout(layoutInfo);
	}

	const vk::RenderPass Pipeline::getRenderPass(const PipelineInfo& pipelineInfo)
	{
		std::vector<vk::AttachmentDescription> attachmentDescriptions;
		std::vector<vk::SubpassDescription> subpassDescriptions;
		/*std::vector<vk::SubpassDependency> dependencies;*/

		for (auto rt : pipelineInfo.renderTargets) {
			attachmentDescriptions.emplace_back(rt->getAttachmentDescription());
		}
		attachmentDescriptions.emplace_back(pipelineInfo.depthBuffer->getAttachmentDescription());
		for (auto inputAttachment : pipelineInfo.inputAttachments) {
			attachmentDescriptions.emplace_back(inputAttachment->getAttachmentDescription());
		}

		//TODO Support Multiple subpasses and dependencies

		std::vector<vk::AttachmentReference> inputAttachmentReferences;
		inputAttachmentReferences.reserve(pipelineInfo.inputAttachments.size());
		for (int i = 0; i < pipelineInfo.inputAttachments.size(); ++i) inputAttachmentReferences.emplace_back(vk::AttachmentReference{ i, pipelineInfo.inputAttachments[i]->getReadLayout() });

		std::vector<vk::AttachmentReference> renderTargetReferences;
		renderTargetReferences.reserve(pipelineInfo.inputAttachments.size());
		for (int i = 0; i < pipelineInfo.renderTargets.size(); ++i) renderTargetReferences.emplace_back(vk::AttachmentReference{ i, pipelineInfo.renderTargets[i]->defaultRenderTargetLayout });

		vk::AttachmentReference depthBufferReference{ 0, DepthBuffer::defaultDepthStencilLayout };

		subpassDescriptions.emplace_back(vk::SubpassDescription{
			{},
			vk::PipelineBindPoint::eGraphics,
			inputAttachmentReferences.size(),
			inputAttachmentReferences.data(),
			renderTargetReferences.size(),
			renderTargetReferences.data(),
			nullptr,
			&depthBufferReference,
			0,
			nullptr
		});


		vk::RenderPassCreateInfo renderPassInfo{
			{},
			attachmentDescriptions.size(),
			attachmentDescriptions.data(),
			subpassDescriptions.size(),
			subpassDescriptions.data(),
			0, 
			nullptr
		};

		return RenderContext::active->getDevice().createRenderPass(renderPassInfo);
	}

	const std::vector<vk::PipelineShaderStageCreateInfo> Pipeline::getShaderInfo(const PipelineInfo& pipelineInfo)
	{

		std::vector<vk::PipelineShaderStageCreateInfo> result;
		result.reserve(pipelineInfo.shaders.size());
		for (auto& shaderInfo : pipelineInfo.shaders) {

			result.emplace_back(vk::PipelineShaderStageCreateInfo{ {}, shaderInfo.first, pipelineHandler->getShaderRaw(shaderInfo.second), "main", nullptr });
		}
		return result;

	}

	PipelineHandler::PipelineHandler(const Configuration& config) : shaderReg(ShaderRegistry(config))
	{
		
		vk::PipelineCacheCreateInfo cacheCreateInfo{};
		pipelineCache = RenderContext::active->getDevice().createPipelineCache(cacheCreateInfo);

		forwardPath = Pipeline{ };
	}
	PipelineHandler::~PipelineHandler()
	{
		const vk::Device device = RenderContext::active->getDevice();

		device.destroyPipelineCache(pipelineCache);
	}
	const vk::PipelineCache& PipelineHandler::getPipelineCache() const
	{
		return pipelineCache;
	}


	const vk::ShaderModule& PipelineHandler::getShaderRaw(const std::string& name) const
	{
		return shaderReg.getShaderRaw(name);
	}

}