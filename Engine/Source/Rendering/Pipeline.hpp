#pragma once

#include "Shader.hpp"
#include "Vertex.hpp"
#include "Image2D.hpp"
#include "VideoSettings.hpp"

#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"

#include "vulkan/vulkan.hpp"

#include <array>


namespace Hyperion::Rendering {

	enum class DepthStencilFlagBits {
		depthTestEnabled = 0x1,
		depthWriteEnable = 0x2, 
		stencilTestEnable = 0x4

	};
	using DepthStencilFlags = Cpp::Flags<DepthStencilFlagBits>;

	enum class BlendMode{
		alphaBlend,
		additiveBlend
	};

	class PipelineInfo {
	public:
		struct DefaultForward {};

		PipelineInfo(DefaultForward forward, std::vector<const FrameBufferAttachment*> attachments, const RenderTarget* renderTarget, const DepthBuffer* depthBuffer) :
			shaders({ {vk::ShaderStageFlagBits::eVertex, "defaultForwardVertex"}, {vk::ShaderStageFlagBits::eFragment, "defaultForwardFragment"} }),
			inputAttachments(attachments),
			renderTargets({renderTarget}),
			depthBuffer(depthBuffer),
			flags(DepthStencilFlags(DepthStencilFlagBits::depthTestEnabled) | DepthStencilFlagBits::depthWriteEnable), //Find more elegant solution for flags
			vertexType(new RawVertex()), // Possibly memory leak
			cullingFlags(vk::CullModeFlagBits::eNone),
			blendMode(BlendMode::alphaBlend)
			{(void) forward;};

		std::vector<std::pair<vk::ShaderStageFlagBits, std::string>> shaders;
		std::vector<const FrameBufferAttachment*> inputAttachments;
		std::vector<const RenderTarget*> renderTargets;
		const DepthBuffer* depthBuffer = nullptr;
		DepthStencilFlags flags = {};
		VertexType* vertexType;
		vk::CullModeFlagBits cullingFlags = vk::CullModeFlagBits::eNone;
		BlendMode blendMode = BlendMode::alphaBlend;

		vk::CompareOp depthCompareOp = vk::CompareOp::eLess;
		vk::CompareOp stencilCompareOp = vk::CompareOp::eLess;
		vk::StencilOp stencilPassOp = vk::StencilOp::eKeep;
		vk::StencilOp stencilFailOp = vk::StencilOp::eKeep;
		vk::StencilOp stencilDepthFailOp = vk::StencilOp::eKeep;
		uint32_t stencilReference = 0;
		vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
		vk::SampleCountFlagBits sampleAmount = vk::SampleCountFlagBits::e1;
		uint32_t tesselationPatchControlPoints = 0;

		vk::Extent3D resolution;
	};

	class PipelineHandler;
	class Pipeline{
	public:
		Pipeline();
		Pipeline(const PipelineHandler& pipelineHandler, const PipelineInfo& pipelineInfo);

		const vk::Pipeline getRaw() const;
		const vk::PipelineLayout& getLayout() const;
		const vk::DescriptorPool& getDescriptorPool() const;
		const std::vector<vk::DescriptorSetLayout>& getDescriptorSetLayouts() const;
		const vk::RenderPass& getRenderPass() const;
		const vk::Framebuffer getActiveFrameBuffer() const;

		static const vk::ClearValue colorClearValue;//vk::ClearColorValue(std::array<float, 4>(0.0f, 0.0f, 0.0f, 1.0f));
		static const vk::ClearValue depthClearValue;//vk::ClearColorValue(std::array<float, 4>(0.0f, 0.0f, 0.0f, 1.0f));

	private:

		vk::PipelineLayout pipelineLayout;
		vk::Pipeline pipeline;
		vk::RenderPass renderpass;
		std::vector<vk::Framebuffer> frameBuffers;
		vk::DescriptorPool descriptorPool;
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;

		const PipelineHandler* pipelineHandler = nullptr;
		static std::unordered_map<std::string, vk::ShaderModule> loadedShaders;

		const std::vector<vk::PipelineShaderStageCreateInfo> getShaderInfo(const PipelineInfo& pipelineInfo);
		const std::vector<vk::Framebuffer> getFrameBuffers(const PipelineInfo& pipelineInfo, const VideoSettings& videoSettings);

		static const vk::RenderPass createRenderPass(const PipelineInfo& pipelineInfo);
		//static const vk::PipelineVertexInputStateCreateInfo getVertexInputInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineTessellationStateCreateInfo createTesselationInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineViewportStateCreateInfo createViewportInfo();
		static const vk::PipelineRasterizationStateCreateInfo createRasterizationInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineMultisampleStateCreateInfo createMultiSampleInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineDepthStencilStateCreateInfo createDepthStencilInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineColorBlendStateCreateInfo createBlendInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineDynamicStateCreateInfo createDynamicStateInfo();
		static const std::pair<vk::PipelineLayout, std::vector<vk::DescriptorSetLayout>> createLayouts(const PipelineInfo& pipelineInfo);
		static const vk::DescriptorPool createDescriptorPool();
	};

	

	class PipelineHandler{
	public:

		PipelineHandler() = default;
		PipelineHandler(const Configuration& config);
		noCopy(PipelineHandler);
		defaultMove(PipelineHandler);
		~PipelineHandler();

		const vk::PipelineCache& getPipelineCache() const;
		const vk::ShaderModule& getShaderRaw(const std::string& name) const;
		const Pipeline& getDefaultForward() const;
	
	private:
		vk::PipelineCache pipelineCache;
		Pipeline forwardPath;
		ShaderRegistry shaderReg;
	};
}