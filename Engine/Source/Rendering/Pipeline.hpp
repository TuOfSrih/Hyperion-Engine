#pragma once

#include "Shader.hpp"
#include "Vertex.hpp"
#include "Image2D.hpp"

#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"

#include "vulkan/vulkan.hpp"


namespace Hyperion::Rendering {

	enum class DepthStencilFlagBits {
		depthTestEnable = 0x1,
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

		std::vector<std::pair<vk::ShaderStageFlagBits, std::string>> shaders;
		std::vector<const FrameBufferAttachment*> inputAttachments;
		std::vector<const RenderTarget*> renderTargets;
		const DepthBuffer* depthBuffer;
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
	};

	class PipelineHandler;
	class Pipeline{
	public:
		Pipeline();
		explicit Pipeline(const PipelineHandler& pipelineHandler, const PipelineInfo& pipelineInfo);

	private:

		static std::unordered_map<std::string, vk::ShaderModule> loadedShaders;
		const PipelineHandler* pipelineHandler = nullptr;
		vk::Pipeline pipeline;
		vk::RenderPass renderpass;

		static const vk::RenderPass getRenderPass(const PipelineInfo& pipelineInfo);
		const std::vector<vk::PipelineShaderStageCreateInfo> getShaderInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineVertexInputStateCreateInfo getVertexInputInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineInputAssemblyStateCreateInfo getInputAssemblyInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineTessellationStateCreateInfo getTesselationInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineViewportStateCreateInfo getViewportInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineRasterizationStateCreateInfo getRasterizationInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineMultisampleStateCreateInfo getMultiSampleInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineDepthStencilStateCreateInfo getDepthStencilInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineColorBlendStateCreateInfo getBlendInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineDynamicStateCreateInfo getDynamicStateInfo(const PipelineInfo& pipelineInfo);
		static const vk::PipelineLayout getPipelineLayout(const PipelineInfo& pipelineInfo);

	};

	

	class PipelineHandler{
	public:

		PipelineHandler() = default;
		PipelineHandler(const Configuration& config);
		~PipelineHandler();

		const vk::PipelineCache& getPipelineCache() const;
		const vk::ShaderModule& getShaderRaw(const std::string& name) const;
	
	private:
		vk::PipelineCache pipelineCache;
		Pipeline forwardPath;
		ShaderRegistry shaderReg;
	};
}