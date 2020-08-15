#pragma once

#include "VideoSettings.hpp"
#include "Image2D.hpp"

#include "System/Configuration.hpp"
#include "Cpp/AdvancedOperators.hpp"

#include "vulkan/vulkan.hpp"

#include <vector>
#include <memory>


namespace Hyperion::Rendering {

	class FrameBuffer {
	public:
		FrameBuffer(const VideoSettings& videoSettings);

		noCopy(FrameBuffer);
		declMove(FrameBuffer);

		~FrameBuffer();

	private:
		std::vector<vk::Framebuffer> framebuffers;
		const DepthBuffer* depthBuffer;
		std::vector<RenderTarget*> renderTargets;

	};
}