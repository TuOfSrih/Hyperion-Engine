#include "vulkan/vulkan.hpp"

#include "RenderContext.hpp"

namespace Hyperion::Rendering {

	class PipelineHandler{

		RenderContext& context;
		vk::PipelineCache pipelineCache;
		vk::Pipeline forwardPath;

		vk::Pipeline createForwardPipeline(const vk::Device& device);
	public:

		PipelineHandler(RenderContext& context);
		~PipelineHandler();
		
	};
}