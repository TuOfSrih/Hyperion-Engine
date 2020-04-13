#pragma once

#include <vector>

#include "vulkan/vulkan.hpp"
#include "Rendering/Queue.hpp"
#include "Rendering/RenderContext.hpp"
#include "Rendering/VulkanUtils.hpp"
#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"


namespace Hyperion::System::Memory {

	
	class VulkanBuffer {
	public:
		enum class Status : uint32_t {
			INVALID = 0,
			VALID,
			ALLOCATED,
			FILLED
		};
		explicit VulkanBuffer() = default;
		explicit VulkanBuffer(
			const void* data,
			const vk::DeviceSize& size,
			const vk::BufferUsageFlags& usageFlags,
			const vk::MemoryPropertyFlags& memoryPropertyFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{});

		declAllTouch(VulkanBuffer);

		~VulkanBuffer();

	private:
		vk::DeviceMemory memory;
		vk::DeviceSize dataSize;
		//TODO Remove allocationsize
		vk::DeviceSize allocationSize;
		vk::Buffer handle;

		Status status;

		static uint32_t findMemoryTypeIndex(const uint32_t typeFilter, const vk::MemoryPropertyFlags memoryPropertyFlags);
	};


	class GPUBuffer {
	public:
		GPUBuffer(const void* data, 
			const vk::DeviceSize& size,
			const vk::BufferUsageFlags& usageFlags,
			const Rendering::Vulkan::SharingInfo& sharingInfo = Rendering::Vulkan::SharingInfo{});

	private:
		VulkanBuffer actualBuffer;
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

	

	
}