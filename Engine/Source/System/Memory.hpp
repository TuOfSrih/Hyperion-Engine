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
		void copyFrom(const VulkanDimResource& other, const vk::ImageLayout thisLayout, const vk::ImageLayout otherLayout, const std::vector<vk::ImageCopy>& imageCopies);
		void copyFrom(const VulkanBuffer& other, const vk::ImageLayout thisLayout, const std::vector<vk::BufferImageCopy>& bufferImageCopies);

		void changeLayout(const Rendering::QueueTypeInfo& queueInfo, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout, const vk::ImageSubresourceRange& subresourceRange);

		vk::ImageLayout currentLayout = vk::ImageLayout::ePreinitialized;

	};

	class GPUDimResource : public VulkanDimResource {
	public:
		GPUDimResource() = default;
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

		noCopy(GPUDimResource);
		defaultMove(GPUDimResource);
		virtual ~GPUDimResource() = default;
		
	};

	
	
}