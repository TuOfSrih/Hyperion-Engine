#pragma once

#include "System/Memory.hpp"


namespace Hyperion::Rendering {

	class Image2D : public System::Memory::GPUDimResource {
	public:
		Image2D() = default;
		Image2D(
			const void* data,
			const vk::DeviceSize size,
			const vk::Format format,
			const vk::Extent2D& extent,
			uint32_t mipLevels,
			const vk::SampleCountFlagBits sampleFlags,
			const vk::ImageUsageFlags imageUsageFlags)
			: GPUDimResource(data, size, {}, vk::ImageType::e2D, format, vk::Extent3D(extent, 1), mipLevels, 1, sampleFlags, imageUsageFlags) {};

		noCopy(Image2D);
		defaultMove(Image2D);

		virtual ~Image2D() = default;

		//Check image formats
		static const vk::Format defaultColorFormat = vk::Format::eB8G8R8A8Unorm;
		static const vk::Format defaultDepthStencilFormat = vk::Format::eD24UnormS8Uint;

		virtual vk::ImageSubresourceRange getTotalResourceRange() const;

	protected:
		vk::Extent2D extent;
	};

	//Put SharingInfo into Template
	class RenderTarget : public Image2D {
	public:
		RenderTarget(
			const vk::Extent2D& extent,
			const void* data = nullptr,
			const vk::DeviceSize size = 0);

		declCopy(RenderTarget);
		defaultMove(RenderTarget);

		~RenderTarget() = default;

		virtual vk::AttachmentDescription getAttachmentDescription(bool lastRenderpass) const override;

		inline static const vk::ImageLayout defaultRenderTargetLayout = vk::ImageLayout::eColorAttachmentOptimal;
		inline static const vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined;
		inline static const vk::ImageLayout presentLayout = vk::ImageLayout::ePresentSrcKHR;

	};

	//Put SharingInfo into Template
	class DepthBuffer : public Image2D {
	public:
		DepthBuffer(
			const vk::Extent2D& extent,
			const void* data = nullptr,
			const vk::DeviceSize size = 0);

		declCopy(DepthBuffer);
		defaultMove(DepthBuffer);

		~DepthBuffer() = default;

		vk::AttachmentDescription getAttachmentDescription(bool lastRenderpass = false) const override;
		vk::ImageSubresourceRange getTotalResourceRange() const override;
		

		inline static const vk::ImageLayout defaultDepthStencilLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		static const float minDepth;
		static const float maxDepth;
	};

	class Texture {

	};

	//TODO Avoid multiple inheritance
	class FrameBufferAttachment {
	public:

		virtual vk::AttachmentDescription getAttachmentDescription() const = 0;
		virtual vk::ImageLayout getReadLayout() const = 0;
		virtual vk::ImageView getImageView() const = 0;
		virtual vk::DescriptorSetLayout getDescriptorSetLayout() const = 0;
	};

}