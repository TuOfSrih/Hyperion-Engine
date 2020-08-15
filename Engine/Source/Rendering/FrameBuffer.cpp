
#include "FrameBuffer.hpp"


namespace Hyperion::Rendering {
	
	FrameBuffer::FrameBuffer(VideoSettings& videoSettings)
	{
		/*const vk::Device device = RenderContext::active->getDevice();
		framebuffers.reserve(videoSettings.bufferImageCount);
		for (int i = 0; i < videoSettings.bufferImageCount; ++i) {

			framebuffers.emplace_back(device.createFramebuffer({

			});
		}*/
	}

	FrameBuffer::~FrameBuffer()
	{
		
	}
}