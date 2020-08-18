
#include "Image2D.hpp"

#include "RenderContext.hpp"

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

	RenderTarget::RenderTarget(const vk::Extent2D& extent, const void* data, const vk::DeviceSize size) : 
		Image2D(data, size, Image2D::defaultColorFormat, extent, 1, vk::SampleCountFlagBits::e1, vk::ImageUsageFlagBits::eColorAttachment)
	{
		changeLayout(QueueType::GRAPHICS, defaultRenderTargetLayout, getTotalResourceRange());
	}

	vk::AttachmentDescription RenderTarget::getAttachmentDescription(bool finalPass) const
	{
		return {
				{},
				Image2D::defaultColorFormat,
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				RenderTarget::initialLayout,
				finalPass? RenderTarget::presentLayout : RenderTarget::defaultRenderTargetLayout
		};
	}

	/*void RenderTarget::makePresentable(const vk::CommandBuffer& cmdBuffer)
	{
		cmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eFragmentShader,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			{},
			{},
			{},
			{
				vk::ImageMemoryBarrier{
					{},
					{},
					currentLayout,
					vk::ImageLayout::ePresentSrcKHR,
					VK_QUEUE_FAMILY_IGNORED,
					VK_QUEUE_FAMILY_IGNORED,
					handle,
					getTotalResourceRange()
				}
			}
		);
	}*/

	DepthBuffer::DepthBuffer(const vk::Extent2D& extent, const void* data, const vk::DeviceSize size)
		: Image2D(data, size, Image2D::defaultDepthStencilFormat, extent, 1, vk::SampleCountFlagBits::e1, vk::ImageUsageFlagBits::eDepthStencilAttachment)
	{
		changeLayout(QueueType::GRAPHICS, defaultDepthStencilLayout, getTotalResourceRange());
	}

	vk::AttachmentDescription DepthBuffer::getAttachmentDescription(bool ) const
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

	vk::ImageSubresourceRange DepthBuffer::getTotalResourceRange() const
	{
		return {
			vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil,
			0,
			1,
			0,
			1
		};
	}

	vk::ImageSubresourceRange Image2D::getTotalResourceRange() const
	{
		return {
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1 
		};
	}
}