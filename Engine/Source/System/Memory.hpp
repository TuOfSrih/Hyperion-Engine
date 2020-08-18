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
			const vk::MemoryPropertyFlags memoryPropertyFlags);
		noTouch(VulkanBuffer);

		virtual ~VulkanBuffer();

		//TODO Find better solution, friend function?
		const vk::Buffer& getHandle() const;
		void* mapMemory() const;
		void  unmapMemory() const;

		static uint32_t findMemoryTypeIndex(const uint32_t typeFilter, const vk::MemoryPropertyFlags memoryPropertyFlags);

		static constexpr vk::SharingMode defaultSharingMode = vk::SharingMode::eExclusive;

	protected:
		//Only for use in constructors and assignments
		void createBuffer(const vk::DeviceSize size, const vk::BufferUsageFlags usageFlags);
		void allocateBuffer(const vk::DeviceSize size, const vk::MemoryPropertyFlags memoryPropertyFlags);
		void fillBuffer(const void* data);
		void copyFrom(const VulkanBuffer& other);
		void stealFrom(VulkanBuffer&& other);
		//Only for use in destructors and assignments
		void release();

		vk::Buffer handle;
		vk::DeviceMemory memory;
		vk::DeviceSize dataSize;
		//TODO Remove allocationsize
		vk::DeviceSize allocationSize;

		ResourceStatus status;

	};


	class GPUBuffer : public VulkanBuffer {
	public:
		GPUBuffer(const void* data, 
			const vk::DeviceSize& size,
			const vk::BufferUsageFlags& usageFlags);

		static constexpr vk::MemoryPropertyFlagBits gpuBufferFlag = vk::MemoryPropertyFlagBits::eDeviceLocal;
		static constexpr vk::BufferUsageFlagBits transferSrcFlag  = vk::BufferUsageFlagBits::eTransferSrc;
		static constexpr vk::BufferUsageFlagBits transferDstFlag  = vk::BufferUsageFlagBits::eTransferDst;
	};

	/*class StridedBuffer : GPUBuffer {
	public:
		StridedBuffer(const void* data, 
			const vk::DeviceSize& size,
			uint32_t stride,
			const vk::BufferUsageFlags& usageFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{})
			: GPUBuffer(data, size, usageFlags, sharingInfo), stride(stride) {};

		uint32_t getStride() const { return stride; };
	protected:
		uint32_t stride;
	};*/
	
	class VertexBuffer : public GPUBuffer {
	public:
		VertexBuffer(const void* data, const vk::DeviceSize& size)
			: GPUBuffer(data, size, vertexBufferFlag) {};
		declAllTouch(VertexBuffer);

		static constexpr vk::BufferUsageFlagBits vertexBufferFlag = vk::BufferUsageFlagBits::eVertexBuffer;
	};

	class IndexBuffer : public GPUBuffer {
	public:
		IndexBuffer(const void* data, const vk::DeviceSize& size)
			: GPUBuffer(data, size, vk::BufferUsageFlagBits::eIndexBuffer) {};
		declAllTouch(IndexBuffer);

		static constexpr vk::IndexType indexType = vk::IndexType::eUint32;
		static constexpr vk::BufferUsageFlagBits indexBufferFlag = vk::BufferUsageFlagBits::eIndexBuffer;
	};

	class VisibleBuffer : public VulkanBuffer {
	public:
		VisibleBuffer(const void* data,
			const vk::DeviceSize& size,
			const vk::BufferUsageFlags& usageFlags)
			: VulkanBuffer(data, size, usageFlags, visibleFlag | coherentFlag) {};

		static constexpr vk::MemoryPropertyFlagBits visibleFlag  = vk::MemoryPropertyFlagBits::eHostVisible;
		static constexpr vk::MemoryPropertyFlagBits coherentFlag = vk::MemoryPropertyFlagBits::eHostCoherent;

	};

	class UniformBuffer : public VisibleBuffer {
	public:
		explicit UniformBuffer(const vk::DeviceSize& size)
			: VisibleBuffer(nullptr, size, uniformBufferFlag) {};
		declAllTouch(UniformBuffer);

		static constexpr vk::BufferUsageFlagBits uniformBufferFlag = vk::BufferUsageFlagBits::eUniformBuffer;
	};

	//TODO Fix RAII as well
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
			const vk::MemoryPropertyFlags memoryPropertyFlags);


		noCopy(VulkanDimResource);
		declMove(VulkanDimResource);

		virtual ~VulkanDimResource();

		//TODO Find better solution, friend function?
		const vk::Image& getHandle() const;
		const vk::ImageView& getImageView() const;
		virtual vk::AttachmentDescription getAttachmentDescription(bool lastRenderpass) const = 0;


		static vk::ImageAspectFlags getImageAspectFlags(const vk::ImageUsageFlags& imageUsageFlags);
		static vk::ImageViewType getViewType(const vk::ImageType& imageType);

	protected:

		vk::Image handle;
		vk::DeviceMemory memory;
		vk::DeviceSize dataSize;
		//TODO Remove allocationsize
		vk::DeviceSize allocationSize;
		vk::ImageView imageView;

		ResourceStatus status;

		void copyFrom(const VulkanDimResource& other, const std::vector<vk::ImageCopy>& imageCopies);
		void copyFrom(const VulkanBuffer& other, const std::vector<vk::BufferImageCopy>& bufferImageCopies);

		void changeLayout(const Rendering::QueueTypeInfo& queueInfo, const vk::ImageLayout& newLayout, const vk::ImageSubresourceRange& subresourceRange);


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
			const vk::ImageUsageFlags imageUsageFlags);

		noCopy(GPUDimResource);
		defaultMove(GPUDimResource);
		virtual ~GPUDimResource() = default;
		
	};

	
	
}