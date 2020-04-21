#pragma once

#include "Rendering/VulkanUtils.hpp"
#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"

#include "vulkan/vulkan.hpp"

#include <vector>


namespace Hyperion::System::Memory {

	enum class ResourceStatus : uint32_t {
		INVALID = 0,
		VALID,
		ALLOCATED,
		FILLED
	};
	
	class VulkanBuffer {
	public:
		
		explicit VulkanBuffer() = default;
		VulkanBuffer(
			const void* data,
			const vk::DeviceSize size,
			const vk::BufferUsageFlags usageFlags,
			const vk::MemoryPropertyFlags memoryPropertyFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{});

		declAllTouch(VulkanBuffer);

		virtual ~VulkanBuffer();

		//TODO Find better solution, friend function?
		const vk::Buffer& getHandle() const;

		static uint32_t findMemoryTypeIndex(const uint32_t typeFilter, const vk::MemoryPropertyFlags memoryPropertyFlags);

	private:
		vk::DeviceMemory memory;
		vk::DeviceSize dataSize;
		//TODO Remove allocationsize
		vk::DeviceSize allocationSize;
		vk::Buffer handle;

		ResourceStatus status;

		
	protected:
		
	};


	class GPUBuffer : VulkanBuffer {
	public:
		GPUBuffer(const void* data, 
			const vk::DeviceSize& size,
			const vk::BufferUsageFlags& usageFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{});
	};
	
	class VertexBuffer : public GPUBuffer{
	public:
		VertexBuffer(const void* data, const vk::DeviceSize& size, const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{})
			: GPUBuffer(data, size, vk::BufferUsageFlagBits::eVertexBuffer, sharingInfo) {};
	};

	class IndexBuffer : public GPUBuffer{
	public:
		IndexBuffer(const void* data, const vk::DeviceSize& size, const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{})
			: GPUBuffer(data, size, vk::BufferUsageFlagBits::eIndexBuffer, sharingInfo) {};
	};

	
	class VulkanDimResource {
	public:

		VulkanDimResource() = default;
		VulkanDimResource(
			const vk::ImageCreateFlags createFlags,
			const vk::ImageType type,
			const vk::Format format,
			const vk::Extent3D& extent,
			uint32_t mipLevels,
			uint32_t arrayLayers,
			const vk::SampleCountFlagBits sampleFlags,
			const vk::ImageTiling imageTiling,
			const vk::ImageUsageFlags imageUsageFlags,
			const vk::MemoryPropertyFlags memoryPropertyFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{});


		noCopy(VulkanDimResource);
		declMove(VulkanDimResource);

		virtual ~VulkanDimResource();

		//TODO Find better solution, friend function?
		const vk::Image& getHandle() const;

	private:
		vk::DeviceMemory memory;
		vk::DeviceSize dataSize;
		//TODO Remove allocationsize
		vk::DeviceSize allocationSize;
		vk::Image handle;

		ResourceStatus status;

	protected:
		void copyFrom(VulkanDimResource&& other, const vk::ImageLayout thisLayout, const vk::ImageLayout otherLayout, const std::vector<vk::ImageCopy>& imageCopies);
		void copyFrom(VulkanBuffer&& other, const vk::ImageLayout thisLayout, const std::vector<vk::BufferImageCopy>& bufferImageCopies);

		void changeLayout(const Rendering::QueueTypeInfo& queueInfo, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout, const vk::ImageSubresourceRange& subresourceRange);

	};

	class GPUDimResource : public VulkanDimResource {
	public:
		GPUDimResource(
			const void* data,
			const vk::DeviceSize size,
			const vk::ImageCreateFlags createFlags,
			const vk::ImageType type,
			const vk::Format format,
			const vk::Extent3D& extent,
			uint32_t mipLevels,
			uint32_t arrayLayers,
			const vk::SampleCountFlagBits sampleFlags,
			const vk::ImageUsageFlags imageUsageFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{});
	};

	class Image2D : public GPUDimResource{
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
	};
	
}