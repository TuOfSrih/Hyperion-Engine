#pragma once

#include "Image2D.hpp"

#include "System/Configuration.hpp"

#include "vulkan/vulkan.hpp"

#include <vector>
#include <memory>


namespace Hyperion::Rendering {

	class FrameBuffer {
	public:
		FrameBuffer(const VideoSettings& videoSettings);

		noCopy();
		declMove();

		~FrameBuffer();

	private:
		std::vector<vk::Framebuffer> framebuffers;
		const DepthBuffer* depthBuffer;
		std::vector<RenderTarget*> renderTargets;

	};
}