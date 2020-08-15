
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
		const vk::Device device = RenderContext::active->getDevice();
		renderpass = createRenderPass(pipelineInfo);
		const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderInfo(pipelineInfo);
		auto attributeDescriptions = pipelineInfo.vertexType->getInputAttributeDescriptions();
		auto bindingDescription = pipelineInfo.vertexType->getInputBindingDescription();
		const vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {//Find better solution
			vk::PipelineVertexInputStateCreateFlags{},
			1,
			&bindingDescription,
			static_cast<uint32_t>(attributeDescriptions.size()),
			attributeDescriptions.data()
		};
		//const vk::PipelineVertexInputStateCreateInfo vertexInputInfo = getVertexInputInfo(pipelineInfo);
		const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = createInputAssemblyInfo(pipelineInfo);
		const vk::PipelineTessellationStateCreateInfo tesselationInfo = createTesselationInfo(pipelineInfo);
		const vk::PipelineViewportStateCreateInfo viewportInfo = createViewportInfo();
		//TODO Better abstraction over parameters
		const vk::PipelineRasterizationStateCreateInfo rasterizationInfo = createRasterizationInfo(pipelineInfo);
		const vk::PipelineMultisampleStateCreateInfo multisampleInfo = createMultiSampleInfo(pipelineInfo);
		const vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = createDepthStencilInfo(pipelineInfo);
		const vk::PipelineColorBlendStateCreateInfo blendInfo = createBlendInfo(pipelineInfo);
		const vk::PipelineDynamicStateCreateInfo dynamicStatesInfo = createDynamicStateInfo();
		const auto[pipeLayout, descriptorLayouts] = createLayouts(pipelineInfo);
		pipelineLayout = pipeLayout;
		this->descriptorSetLayouts = descriptorLayouts;//TODO Move

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

		descriptorPool = createDescriptorPool();
	}

	const vk::Pipeline Pipeline::getRaw() const
	{
		return pipeline;
	}

	const vk::PipelineLayout& Pipeline::getLayout() const
	{
		return pipelineLayout;
	}

	const vk::DescriptorPool& Pipeline::getDescriptorPool() const
	{
		return descriptorPool;
	}

	const std::vector<vk::DescriptorSetLayout>& Pipeline::getDescriptorSetLayouts() const
	{
		return descriptorSetLayouts;
	}

	const vk::RenderPass& Pipeline::getRenderPass() const
	{
		return renderpass;
	}

	const vk::Framebuffer Pipeline::getActiveFrameBuffer() const
	{

		return frameBuffers.at(RenderContext::active->getActiveBufferIndex());
	}

	const vk::ClearValue Pipeline::colorClearValue = vk::ClearColorValue{ std::array<float,4> { 0.0f, 0.0f, 0.0f, 1.0f } };
	const vk::ClearValue Pipeline::depthClearValue = vk::ClearDepthStencilValue{ 1.0f, 0 };

	//const vk::PipelineVertexInputStateCreateInfo Pipeline::getVertexInputInfo(const PipelineInfo& pipelineInfo) {

	//	//Dont return reference to local variable
	//	
	//}

	const vk::PipelineInputAssemblyStateCreateInfo Pipeline::createInputAssemblyInfo(const PipelineInfo& pipelineInfo) {

		return vk::PipelineInputAssemblyStateCreateInfo{
				{},
				pipelineInfo.topology,
				VK_FALSE//Revisit later
		};
	}
	
	const vk::PipelineTessellationStateCreateInfo Pipeline::createTesselationInfo (const PipelineInfo& pipelineInfo) {

		return vk::PipelineTessellationStateCreateInfo {
			{},
			pipelineInfo.tesselationPatchControlPoints
		};
	}

	const vk::PipelineViewportStateCreateInfo Pipeline::createViewportInfo() {

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

	const vk::PipelineRasterizationStateCreateInfo Pipeline::createRasterizationInfo(const PipelineInfo& pipelineInfo) {

		/*bool isLine = pipelineInfo.topology == vk::PrimitiveTopology::eLineList ||
			pipelineInfo.topology == vk::PrimitiveTopology::eLineListWithAdjacency ||
			pipelineInfo.topology == vk::PrimitiveTopology::eLineStrip;*/

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
			(isTriangle || isPoint ? vk::PolygonMode::eFill : vk::PolygonMode::eLine),
			pipelineInfo.cullingFlags,
			RenderOrder,
			VK_FALSE,
		};
	}

	const vk::PipelineMultisampleStateCreateInfo Pipeline::createMultiSampleInfo(const PipelineInfo& pipelineInfo) {

		return vk::PipelineMultisampleStateCreateInfo{
			{},
			pipelineInfo.sampleAmount,
		};
	}

	const vk::PipelineDepthStencilStateCreateInfo Pipeline::createDepthStencilInfo(const PipelineInfo& pipelineInfo) {

		return vk::PipelineDepthStencilStateCreateInfo{ 
			{},
			pipelineInfo.flags & DepthStencilFlagBits::depthTestEnabled,
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

	const vk::PipelineColorBlendStateCreateInfo Pipeline::createBlendInfo(const PipelineInfo& pipelineInfo) {


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

	const vk::PipelineDynamicStateCreateInfo Pipeline::createDynamicStateInfo() {

		const std::array<vk::DynamicState, 2> dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		return vk::PipelineDynamicStateCreateInfo{
			{}, static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data()
		};
	}

	const std::pair<vk::PipelineLayout, std::vector<vk::DescriptorSetLayout>> Pipeline::createLayouts(const PipelineInfo& pipelineInfo)
	{
		const vk::Device device = RenderContext::active->getDevice();

		std::vector<vk::DescriptorSetLayout> descriptorLayoutInfos;
		descriptorLayoutInfos.reserve(2);
		pipelineInfo.vertexType->getDescriptorLayout();

		for (auto& inputAtt : pipelineInfo.inputAttachments) descriptorLayoutInfos.emplace_back(inputAtt->getDescriptorSetLayout());
		
		vk::PipelineLayoutCreateInfo layoutInfo{
			{},
			static_cast<uint32_t>(descriptorLayoutInfos.size()),
			descriptorLayoutInfos.data(),
			0, 
			nullptr
		};
		return std::make_pair(device.createPipelineLayout(layoutInfo), descriptorLayoutInfos);
	}

	const vk::DescriptorPool Pipeline::createDescriptorPool()
	{
		const vk::Device& device = RenderContext::active->getDevice();

		std::vector<vk::DescriptorPoolSize> poolSizes = { {vk::DescriptorType::eUniformBuffer, RenderContext::active->getVideoSettings().bufferImageCount } };

		//for(auto& texture: pipelineInfo.in)TODO Add descriptors for texturing and input attachments

		return device.createDescriptorPool(
			vk::DescriptorPoolCreateInfo{
				{},
				RenderContext::active->getVideoSettings().bufferImageCount,
				static_cast<uint32_t>(poolSizes.size()),
				poolSizes.data()
			}
		);
	}

	const std::vector<vk::Framebuffer> Pipeline::getFrameBuffers(const PipelineInfo& pipelineInfo, const VideoSettings& videoSettings)
	{
		const vk::Device& device = RenderContext::active->getDevice();
		frameBuffers.reserve(videoSettings.bufferImageCount);
		std::vector<vk::ImageView> imageViews;
		imageViews.reserve((pipelineInfo.depthBuffer != nullptr) + pipelineInfo.inputAttachments.size() + pipelineInfo.renderTargets.size());
		if (pipelineInfo.depthBuffer != nullptr) imageViews.push_back(pipelineInfo.depthBuffer->getImageView());
		for (auto& rt : pipelineInfo.renderTargets) imageViews.push_back(rt->getImageView());
		for (auto& input : pipelineInfo.inputAttachments) imageViews.push_back(input->getImageView());

		for (uint8_t i = 0; i < videoSettings.bufferImageCount; ++i) {
			
			frameBuffers.push_back(device.createFramebuffer({
				{},
				renderpass,
				static_cast<uint32_t>(imageViews.size()),
				imageViews.data(),
				pipelineInfo.resolution.width,
				pipelineInfo.resolution.height,
				pipelineInfo.resolution.depth,
				
			}));
		}
		return frameBuffers;
	}

	const vk::RenderPass Pipeline::createRenderPass(const PipelineInfo& pipelineInfo)
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
		for (uint32_t i = 0; i < pipelineInfo.inputAttachments.size(); ++i) inputAttachmentReferences.emplace_back(vk::AttachmentReference{ i, pipelineInfo.inputAttachments[i]->getReadLayout() });

		std::vector<vk::AttachmentReference> renderTargetReferences;
		renderTargetReferences.reserve(pipelineInfo.inputAttachments.size());
		for (uint32_t i = 0; i < pipelineInfo.renderTargets.size(); ++i) renderTargetReferences.emplace_back(vk::AttachmentReference{ i, pipelineInfo.renderTargets[i]->defaultRenderTargetLayout });

		vk::AttachmentReference depthBufferReference{ 0, DepthBuffer::defaultDepthStencilLayout };

		subpassDescriptions.emplace_back(vk::SubpassDescription{
			{},
			vk::PipelineBindPoint::eGraphics,
			static_cast<uint32_t>(inputAttachmentReferences.size()),
			inputAttachmentReferences.data(),
			static_cast<uint32_t>(renderTargetReferences.size()),
			renderTargetReferences.data(),
			nullptr,
			&depthBufferReference,
			0,
			nullptr
		});


		vk::RenderPassCreateInfo renderPassInfo{
			{},
			static_cast<uint32_t>(attachmentDescriptions.size()),
			attachmentDescriptions.data(),
			static_cast<uint32_t>(subpassDescriptions.size()),
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

	const Pipeline& PipelineHandler::getDefaultForward() const
	{
		return forwardPath;
	}

}