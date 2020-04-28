#pragma once

#include "RenderContext.hpp"

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
			const vk::ImageUsageFlags imageUsageFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{})
			: GPUDimResource(data, size, {}, vk::ImageType::e2D, format, vk::Extent3D(extent, 1), mipLevels, 1, sampleFlags, imageUsageFlags, sharingInfo) {};

		noCopy(Image2D);
		defaultMove(Image2D);

		virtual ~Image2D() = default;

		//Check image formats
		static const vk::Format defaultColorFormat = vk::Format::eR8G8B8A8Unorm;
		static const vk::Format defaultDepthStencilFormat = vk::Format::eD24UnormS8Uint;
	protected:
		vk::Extent2D extent;
	};

	//Put SharingInfo into Template
	class RenderTarget : public Image2D {
	public:
		RenderTarget(
			const vk::Extent2D& extent = RenderContext::active->getVideoSettings().resolution,
			const void* data = nullptr,
			const vk::DeviceSize size = 0,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{})
			: Image2D(data, size, Image2D::defaultColorFormat, extent, 1, vk::SampleCountFlagBits::e1, vk::ImageUsageFlagBits::eColorAttachment, sharingInfo) {};

		declCopy(RenderTarget);
		defaultMove(RenderTarget);

		~RenderTarget() = default;

		virtual vk::AttachmentDescription getAttachmentDescription() const override;

		static const vk::ImageLayout defaultRenderTargetLayout = vk::ImageLayout::eColorAttachmentOptimal;

	};

	//Put SharingInfo into Template
	class DepthBuffer : public Image2D {
	public:
		DepthBuffer(
			const vk::Extent2D& extent = RenderContext::active->getVideoSettings().resolution,
			const void* data = nullptr,
			const vk::DeviceSize size = 0,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{})
			: Image2D(data, size, Image2D::defaultDepthStencilFormat, extent, 1, vk::SampleCountFlagBits::e1, vk::ImageUsageFlagBits::eDepthStencilAttachment, sharingInfo) {};

		declCopy(DepthBuffer);
		defaultMove(DepthBuffer);

		~DepthBuffer() = default;

		virtual vk::AttachmentDescription getAttachmentDescription() const override;

		static const vk::ImageLayout defaultDepthStencilLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	};

	class Texture {

	};

	class FrameBufferAttachment {
	public:

		virtual vk::AttachmentDescription getAttachmentDescription() const = 0;
		virtual vk::ImageLayout getReadLayout() const = 0;
		virtual vk::DescriptorSetLayout getDescriptorSetLayout() const = 0;
	};

}