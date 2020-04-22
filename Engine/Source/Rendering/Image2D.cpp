
#include "Image2D.hpp"


namespace Hyperion::Rendering {


	RenderTarget::RenderTarget(const RenderTarget& other) {

		*this = other;
	}

	RenderTarget& RenderTarget::operator=(const RenderTarget& other) {

		ASSERT(extent == other.extent);
		copyFrom(other, currentLayout, other.currentLayout,
			{
				{
					{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
					{},
					{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
					{},
					vk::Extent3D(extent, 1)
				}
			}
		);

		return *this;
	}

	DepthBuffer::DepthBuffer(const DepthBuffer& other) {

		*this = other;
	}

	DepthBuffer& DepthBuffer::operator=(const DepthBuffer& other) {
		
		ASSERT(extent == other.extent);
		copyFrom(other, currentLayout, other.currentLayout,
			{
				{
					{vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 0, 1},
					{},
					{vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 0, 1},
					{},
					vk::Extent3D(extent, 1)
				}
			}
		);

		return *this;
	}
}