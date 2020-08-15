
#include "Image2D.hpp"

#include "System/Debug.hpp"


namespace Hyperion::Rendering {


	RenderTarget::RenderTarget(const RenderTarget& other) {

		*this = other;
	}

	RenderTarget& RenderTarget::operator=(const RenderTarget& other) {

		ASSERT(extent == other.extent);
		copyFrom(other,
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
		copyFrom(other,
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

	const float DepthBuffer::minDepth = .0f;
	const float DepthBuffer::maxDepth = 1.f;

	vk::AttachmentDescription RenderTarget::getAttachmentDescription() const 
	{
		return {
				{},
				Image2D::defaultColorFormat,
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eLoad,
				vk::AttachmentStoreOp::eDontCare,
				vk::AttachmentLoadOp::eLoad,
				vk::AttachmentStoreOp::eDontCare,
				RenderTarget::defaultRenderTargetLayout,
				RenderTarget::defaultRenderTargetLayout
		};
	}
	vk::AttachmentDescription DepthBuffer::getAttachmentDescription() const
	{
		return {
				{},
				Image2D::defaultDepthStencilFormat,
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eLoad,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eLoad,
				vk::AttachmentStoreOp::eStore,
				DepthBuffer::defaultDepthStencilLayout,
				DepthBuffer::defaultDepthStencilLayout
		};
	}
}