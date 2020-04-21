
#include "Memory.hpp"

#include "System/Debug.hpp"
#include "Rendering/RenderContext.hpp"

namespace Hyperion::System::Memory {
	
	VulkanBuffer::VulkanBuffer(const void* data, const vk::DeviceSize size, const vk::BufferUsageFlags usageFlags,
		const vk::MemoryPropertyFlags memoryPropertyFlags,
		const Rendering::Vulkan::SharingInfo& sharingInfo)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();

		handle = device.createBuffer(
			vk::BufferCreateInfo{
				vk::BufferCreateFlags{},
				size,
				usageFlags,
				sharingInfo.getSharingMode(),
				static_cast<uint32_t>(sharingInfo.getAmount()),
				sharingInfo.getIndices()
			}
		);
		status = ResourceStatus::VALID;

		if (size > 0) {
			vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(handle);
			allocationSize = std::max(memRequirements.size, size);
			memory = device.allocateMemory(
				vk::MemoryAllocateInfo{
					allocationSize,
					findMemoryTypeIndex(memRequirements.memoryTypeBits, memoryPropertyFlags)
				}
			);

			device.bindBufferMemory(handle, memory, 0);

			status = ResourceStatus::ALLOCATED;
		} else allocationSize = 0;
		
		if (data != nullptr) {

			void* dataCopy;
			device.mapMemory(memory, 0, size, {}, &dataCopy);
			std::memcpy(dataCopy, data, static_cast<size_t>(size));
			device.unmapMemory(memory);

			status = ResourceStatus::FILLED;
		}


	}

	VulkanBuffer::VulkanBuffer(const VulkanBuffer& other) {

		*this = other;
	}

	VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) {

		*this = std::move(other);
	}

	VulkanBuffer& VulkanBuffer::operator=(const VulkanBuffer& other) {

		
		//Check C++ Lambdas
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		vk::CommandBuffer buffer = Hyperion::Rendering::Vulkan::simpleExecuteTransfer([this, &other](const vk::CommandBuffer& cmdBuffer) {
				cmdBuffer.copyBuffer(
					other.handle, 
					this->handle,
					{
						0, 
						0, 
						other.dataSize
					}
				);
			}
		);
		//Todo Implement proper handler with callback
		Rendering::RenderContext::active->getTransferQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);

		return *this;
	}

	VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) {

		if (this != &other) {

			this->~VulkanBuffer();

			dataSize = std::move(other.dataSize);
			memory = std::move(other.memory);
			handle = std::move(other.handle);
			status = std::move(other.status);

			other.dataSize = 0;
			other.memory = vk::DeviceMemory();
			other.handle = vk::Buffer();
			other.status = ResourceStatus::INVALID;
		}
		
		
		return *this;
	}

	VulkanBuffer::~VulkanBuffer()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		if (status >= ResourceStatus::ALLOCATED)  device.freeMemory(memory);
		if (status >= ResourceStatus::VALID) device.destroyBuffer(handle);
	}

	uint32_t VulkanBuffer::findMemoryTypeIndex(const uint32_t typeFilter, const vk::MemoryPropertyFlags memoryPropertyFlags)
	{
		vk::PhysicalDeviceMemoryProperties memoryProperties = Rendering::RenderContext::active->getGPU().getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {

			if ((typeFilter & i << i) && memoryProperties.memoryTypes[i].propertyFlags == memoryPropertyFlags) {

				return i;
			}
		}

		Debug::missingFunctionality("Could not find usable memory type");
	}

	const vk::Buffer& VulkanBuffer::getHandle() const
	{
		return handle;
	}

	GPUBuffer::GPUBuffer(const void* data, const vk::DeviceSize& size, const vk::BufferUsageFlags& usageFlags, const Rendering::Vulkan::SharingInfo& sharingInfo)
		: VulkanBuffer(nullptr, size,  (data != nullptr ? usageFlags | vk::BufferUsageFlagBits::eTransferDst : usageFlags), vk::MemoryPropertyFlagBits::eDeviceLocal, sharingInfo)
	{
		if (data != nullptr) {
			VulkanBuffer stagingBuffer(data, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, sharingInfo);

			*static_cast<VulkanBuffer*>(this) = stagingBuffer;
		}
		
	}
	

	VulkanDimResource::VulkanDimResource(
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
		const Rendering::Vulkan::SharingInfo& sharingInfo)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();

		handle = device.createImage(vk::ImageCreateInfo{
			createFlags,
			type,
			format,
			extent,
			mipLevels,
			arrayLayers,
			sampleFlags,
			imageTiling,
			imageUsageFlags,
			sharingInfo.getSharingMode(),
			static_cast<uint32_t>(sharingInfo.getAmount()),
			sharingInfo.getIndices(),
			vk::ImageLayout::ePreinitialized // vs undefined, especially for GPU Resource
		});
		status = ResourceStatus::VALID;

		vk::MemoryRequirements memRequirements = device.getImageMemoryRequirements(handle);
		if (memRequirements.size > 0) {
			
			memory = device.allocateMemory({
				memRequirements.size,
				VulkanBuffer::findMemoryTypeIndex(memRequirements.memoryTypeBits, memoryPropertyFlags)
			});

			device.bindImageMemory(handle, memory, 0);

			status = ResourceStatus::ALLOCATED;

		}
	}

	VulkanDimResource::VulkanDimResource(VulkanDimResource&& other) {
		*this = std::move(other);
	}


	VulkanDimResource& VulkanDimResource::operator=(VulkanDimResource&& other) {

		if (this != &other) {

			this->~VulkanDimResource();

			dataSize = std::move(other.dataSize);
			memory = std::move(other.memory);
			handle = std::move(other.handle);
			status = std::move(other.status);

			other.dataSize = 0;
			other.memory = vk::DeviceMemory();
			other.handle = vk::Image();
			other.status = ResourceStatus::INVALID;
		}

		return *this;
	}

	VulkanDimResource::~VulkanDimResource()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		if (status >= ResourceStatus::ALLOCATED) device.freeMemory(memory);
		if (status >= ResourceStatus::VALID) device.destroyImage(handle);
	}

	const vk::Image& VulkanDimResource::getHandle() const
	{
		return handle;
	}


	void VulkanDimResource::copyFrom(VulkanDimResource&& other, const vk::ImageLayout thisLayout, const vk::ImageLayout otherLayout, const std::vector<vk::ImageCopy>& imageCopies)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		ASSERT(status >= ResourceStatus::ALLOCATED);
		vk::CommandBuffer buffer = Hyperion::Rendering::Vulkan::simpleExecuteTransfer([&](const vk::CommandBuffer& cmdBuffer) {
			cmdBuffer.copyImage(
				other.handle,
				otherLayout,
				handle,
				thisLayout,
				imageCopies
			);
		}
		);
		//Todo Implement proper handler with callback
		Rendering::RenderContext::active->getTransferQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);
	}

	void VulkanDimResource::copyFrom(VulkanBuffer&& other, const vk::ImageLayout thisLayout, const std::vector<vk::BufferImageCopy>& bufferImageCopies)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		ASSERT(status >= ResourceStatus::ALLOCATED);
		vk::CommandBuffer buffer = Rendering::Vulkan::simpleExecuteTransfer([&](const vk::CommandBuffer& cmdBuffer) {
			cmdBuffer.copyBufferToImage(
				other.getHandle(),
				handle,
				thisLayout,
				bufferImageCopies
			);
		}
		);
		//Todo Implement proper handler with callback
		Rendering::RenderContext::active->getTransferQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);
	}

	void VulkanDimResource::changeLayout(const Rendering::QueueTypeInfo& queueInfo, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout, const vk::ImageSubresourceRange& subresourceRange)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		ASSERT(static_cast<uint32_t>(status) >= static_cast<uint32_t>(ResourceStatus::VALID));
		vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::AccessFlags srcFlags;
		vk::AccessFlags dstFlags;
		if (newLayout == vk::ImageLayout::eTransferDstOptimal) {
			
			dstStage = vk::PipelineStageFlagBits::eTransfer;

			srcFlags = {};
			dstFlags |= vk::AccessFlagBits::eTransferWrite;
		}
		//TODO Add more cases or generally more elaborate process

		//Check C++ Lambdas
		vk::CommandBuffer buffer = Rendering::Vulkan::simpleExecute(queueInfo, [&, this](const vk::CommandBuffer& cmdBuffer) {
			cmdBuffer.pipelineBarrier(
				srcStage,
				dstStage,
				{},
				{},
				{},
				{
					vk::ImageMemoryBarrier{
						srcFlags,
						dstFlags,
						oldLayout,
						newLayout,
						VK_QUEUE_FAMILY_IGNORED,
						VK_QUEUE_FAMILY_IGNORED,
						handle,
						subresourceRange
					}
				}
				);
		});

		//Todo Implement proper handler with callback
		queueInfo.getQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);
		
	}

	GPUDimResource::GPUDimResource(const void* data,
		const vk::DeviceSize size,
		const vk::ImageCreateFlags createFlags,
		const vk::ImageType type,
		const vk::Format format,
		const vk::Extent3D& extent,
		uint32_t mipLevels,
		uint32_t arrayLayers,
		const vk::SampleCountFlagBits sampleFlags,
		const vk::ImageUsageFlags imageUsageFlags,
		const Rendering::Vulkan::SharingInfo& sharingInfo) : VulkanDimResource(createFlags, type, format, extent, mipLevels, arrayLayers, sampleFlags, vk::ImageTiling::eOptimal, imageUsageFlags, vk::MemoryPropertyFlagBits::eDeviceLocal) {

		if (data != nullptr) {
			VulkanBuffer stagingBuffer(data, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, sharingInfo);
			
			vk::ImageAspectFlags flags; 
			flags |= (imageUsageFlags & vk::ImageUsageFlagBits::eColorAttachment) ? vk::ImageAspectFlagBits::eColor : vk::ImageAspectFlagBits{};
			flags |= (imageUsageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment) ?
				vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil : vk::ImageAspectFlagBits{};

			changeLayout(Rendering::QueueType::TRANSFER, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal, { flags, 0, mipLevels, 0, arrayLayers });
			copyFrom(std::move(stagingBuffer), vk::ImageLayout::ePreinitialized, { { 0, 0, 0, {flags, 0, 0, arrayLayers}, {}, extent } });

		}
	}

}


