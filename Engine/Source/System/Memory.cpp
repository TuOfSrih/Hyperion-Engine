#include "Memory.hpp"
#include "Rendering/RenderContext.hpp"
#include "System/Debug.hpp"

namespace Hyperion::System::Memory {
	
	VulkanBuffer::VulkanBuffer(const void* data, const vk::DeviceSize& size, const vk::BufferUsageFlags& usageFlags,
		const vk::MemoryPropertyFlags& memoryPropertyFlags,
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
		status = Status::VALID;

		if (size > 0) {
			memory = device.allocateMemory(
				vk::MemoryAllocateInfo{
					size,
					findMemoryTypeIndex(device.getBufferMemoryRequirements(handle).memoryTypeBits, memoryPropertyFlags)
				}
			);

			device.bindBufferMemory(handle, memory, 0);

			status = Status::ALLOCATED;
		}
		
		if (data != nullptr) {

			void* dataCopy;
			device.mapMemory(memory, 0, size, {}, &dataCopy);
			std::memcpy(dataCopy, data, static_cast<size_t>(size));
			device.unmapMemory(memory);
			status = Status::FILLED;
		}


	}

	VulkanBuffer::VulkanBuffer(const VulkanBuffer& other) {

		*this = other;
	}

	VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) {

		*this = std::move(other);
	}

	VulkanBuffer& VulkanBuffer::operator=(const VulkanBuffer& other) {

		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		vk::CommandBuffer buffer = Hyperion::Rendering::Vulkan::simpleExecuteTransfer([this, &other](const vk::CommandBuffer& cmdBuffer) {
				cmdBuffer.copyBuffer(
					other.handle, 
					this->handle,
					{
						0, 
						0, 
						other.size
					}
				);
			}
		);
		//Todo Implement proper handler with callback
		Rendering::RenderContext::active->getTransferQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);
		throw std::exception();
	}

	VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) {

		this->~VulkanBuffer();

		size = other.size;
		memory = other.memory;
		handle = other.handle;
		status = other.status;
		
		return *this;
	}

	VulkanBuffer::~VulkanBuffer()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		if (status >= Status::ALLOCATED)  device.freeMemory(memory);
		if (status >= Status::VALID) device.destroyBuffer(handle);
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
		return 0;
	}

	GPUBuffer::GPUBuffer(const void* data, const vk::DeviceSize& size, const vk::BufferUsageFlags& usageFlags, const Rendering::Vulkan::SharingInfo& sharingInfo)
		: actualBuffer(VulkanBuffer(nullptr, size, usageFlags | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, sharingInfo))
	{

		VulkanBuffer stagingBuffer(data, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, sharingInfo);
		
		actualBuffer = stagingBuffer;
	}
	
}


