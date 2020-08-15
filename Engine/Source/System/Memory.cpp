
#include "Memory.hpp"

#include "System/Debug.hpp"
#include "Rendering/RenderContext.hpp"

namespace Hyperion::System::Memory {
	
	VulkanBuffer::VulkanBuffer(const void* data, const vk::DeviceSize size, const vk::BufferUsageFlags usageFlags,
		const vk::MemoryPropertyFlags memoryPropertyFlags): dataSize(size)
	{

		createBuffer(size, usageFlags);
		allocateBuffer(size, memoryPropertyFlags);
		fillBuffer(data);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		release();
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

	void VulkanBuffer::createBuffer(const vk::DeviceSize size, const vk::BufferUsageFlags usageFlags)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		handle = device.createBuffer(
			vk::BufferCreateInfo{
				vk::BufferCreateFlags{},
				size,
				usageFlags,
				defaultSharingMode,
				0,
				nullptr
			}
		);
		status = ResourceStatus::VALID;
	}

	void VulkanBuffer::allocateBuffer(const vk::DeviceSize size, const vk::MemoryPropertyFlags memoryPropertyFlags)
	{
		if (size > 0) {

			const vk::Device& device = Rendering::RenderContext::active->getDevice();
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
		}
		else allocationSize = 0;
	}

	void VulkanBuffer::fillBuffer(const void* data)
	{
		if (data != nullptr) {

			void* dataCopy = mapMemory();
			std::memcpy(dataCopy, data, static_cast<size_t>(dataSize));
			unmapMemory();

			status = ResourceStatus::FILLED;
		}
	}

	void VulkanBuffer::release()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		if (status >= ResourceStatus::ALLOCATED)  device.freeMemory(memory);
		if (status >= ResourceStatus::VALID) device.destroyBuffer(handle);
		
		memory = vk::DeviceMemory();
		handle = vk::Buffer();
		dataSize = 0;
		allocationSize = 0;
		status = ResourceStatus::INVALID;
	}

	void VulkanBuffer::copyFrom(const VulkanBuffer& other)
	{
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
	}

	void VulkanBuffer::stealFrom(VulkanBuffer&& other)
	{
		memory = std::move(other.memory);
		handle = std::move(other.handle);
		dataSize = std::move(other.dataSize);
		allocationSize = std::move(other.allocationSize);
		status = std::move(other.status);

		other.handle = vk::Buffer();
		other.memory = vk::DeviceMemory();
		other.dataSize = 0;
		other.allocationSize = 0;
		other.status = ResourceStatus::INVALID;
	}

	const vk::Buffer& VulkanBuffer::getHandle() const
	{
		return handle;
	}

	void* VulkanBuffer::mapMemory() const
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		void* ptr;
		device.mapMemory(memory, 0, dataSize, {}, &ptr);
		return ptr;
	}

	void VulkanBuffer::unmapMemory() const
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		device.unmapMemory(memory);
	}


	GPUBuffer::GPUBuffer(const void* data, const vk::DeviceSize& size, const vk::BufferUsageFlags& usageFlags)
		: VulkanBuffer(nullptr, size,  (data != nullptr ? usageFlags | transferDstFlag : usageFlags), gpuBufferFlag)
	{
		if (data != nullptr) {
			VulkanBuffer stagingBuffer(data, size, transferSrcFlag,
				VisibleBuffer::visibleFlag | VisibleBuffer::coherentFlag);

			copyFrom(stagingBuffer);
		}
		
	}

	VertexBuffer::VertexBuffer(const VertexBuffer& other): GPUBuffer(nullptr, other.dataSize, vertexBufferFlag) {

		copyFrom(other);
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other) : GPUBuffer(nullptr, other.dataSize, vertexBufferFlag) {

		stealFrom(std::move(other));
	}

	VertexBuffer& VertexBuffer::operator=(const VertexBuffer& other) {

		if (this != &other) {

			//Do not release as that would loose the buffer reference
			copyFrom(other);
		}

		return *this;
	}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) {

		if (this != &other) {

			release();
			stealFrom(std::move(other));
		}

		return *this;
	}

	IndexBuffer::IndexBuffer(const IndexBuffer& other) : IndexBuffer(nullptr, other.dataSize) {

		copyFrom(other);
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other) : IndexBuffer(nullptr, other.dataSize) {

		stealFrom(std::move(other));
	}

	IndexBuffer& IndexBuffer::operator=(const IndexBuffer& other) {

		if (this != &other) {

			//Do not release as that would loose the buffer reference
			copyFrom(other);
		}

		return *this;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) {

		if (this != &other) {

			release();
			stealFrom(std::move(other));
		}

		return *this;
	}

	UniformBuffer::UniformBuffer(const UniformBuffer& other) : UniformBuffer(other.dataSize) {

		copyFrom(other);
	}

	UniformBuffer::UniformBuffer(UniformBuffer&& other) : UniformBuffer(other.dataSize) {

		stealFrom(std::move(other));
	}

	UniformBuffer& UniformBuffer::operator=(const UniformBuffer& other) {

		if (this != &other) {

			//Do not release as that would lose the buffer reference
			copyFrom(other);
		}

		return *this;
	}

	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) {

		if (this != &other) {

			release();
			stealFrom(std::move(other));
		}

		return *this;
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
		const vk::MemoryPropertyFlags memoryPropertyFlags)
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
			VulkanBuffer::defaultSharingMode,
			0,
			nullptr,
			currentLayout // vs undefined, especially for GPU Resource
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

		imageView = device.createImageView(vk::ImageViewCreateInfo{
			{},
			handle,
			VulkanDimResource::getViewType(type),
			format,
			{},
			vk::ImageSubresourceRange{
				VulkanDimResource::getImageAspectFlags(imageUsageFlags),
				0, 
				mipLevels,
				0,
				arrayLayers
			}
		});
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
			imageView = std::move(other.imageView);

			other.dataSize = 0;
			other.memory = vk::DeviceMemory();
			other.handle = vk::Image();
			other.status = ResourceStatus::INVALID;
			other.imageView = vk::ImageView();
		}

		return *this;
	}

	VulkanDimResource::~VulkanDimResource()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();

		device.destroyImageView(imageView);
		if (status >= ResourceStatus::ALLOCATED) device.freeMemory(memory);
		if (status >= ResourceStatus::VALID) device.destroyImage(handle);
	}

	vk::ImageAspectFlags VulkanDimResource::getImageAspectFlags(const vk::ImageUsageFlags& imageUsageFlags)
	{
		vk::ImageAspectFlags flags;
		flags |= (imageUsageFlags & vk::ImageUsageFlagBits::eColorAttachment) ? vk::ImageAspectFlagBits::eColor : vk::ImageAspectFlagBits{};
		flags |= (imageUsageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment) ?
			vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil : vk::ImageAspectFlagBits{};

		return flags;
	}

	vk::ImageViewType VulkanDimResource::getViewType(const vk::ImageType& imageType)
	{
		return static_cast<vk::ImageViewType>(imageType);
	}

	const vk::Image& VulkanDimResource::getHandle() const
	{
		return handle;
	}

	const vk::ImageView& VulkanDimResource::getImageView() const
	{
		return imageView;
	}

	void VulkanDimResource::copyFrom(const VulkanDimResource& other, const std::vector<vk::ImageCopy>& imageCopies)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		ASSERT(status >= ResourceStatus::ALLOCATED);
		vk::CommandBuffer buffer = Hyperion::Rendering::Vulkan::simpleExecuteTransfer([&](const vk::CommandBuffer& cmdBuffer) {
			cmdBuffer.copyImage(
				other.handle,
				other.currentLayout,
				handle,
				currentLayout,
				imageCopies
			);
		}
		);
		//Todo Implement proper handler with callback
		Rendering::RenderContext::active->getTransferQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);
	}

	void VulkanDimResource::copyFrom(const VulkanBuffer& other, const std::vector<vk::BufferImageCopy>& bufferImageCopies)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		ASSERT(status >= ResourceStatus::ALLOCATED);
		vk::CommandBuffer buffer = Rendering::Vulkan::simpleExecuteTransfer([&](const vk::CommandBuffer& cmdBuffer) {
			cmdBuffer.copyBufferToImage(
				other.getHandle(),
				handle,
				currentLayout,
				bufferImageCopies
			);
		});
		//Todo Implement proper handler with callback
		Rendering::RenderContext::active->getTransferQueue().waitIdle();
		device.freeCommandBuffers(Rendering::RenderContext::active->getTransferPool(), 1, &buffer);
	}

	void VulkanDimResource::changeLayout(const Rendering::QueueTypeInfo& queueInfo, const vk::ImageLayout& newLayout, const vk::ImageSubresourceRange& subresourceRange)
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
						currentLayout,
						newLayout,
						VK_QUEUE_FAMILY_IGNORED,
						VK_QUEUE_FAMILY_IGNORED,
						handle,
						subresourceRange
					}
				}
				);
		});

		currentLayout = newLayout;

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
		const vk::ImageUsageFlags imageUsageFlags//Check Image Tiling with optimal and linear
		) : VulkanDimResource(createFlags, type, format, extent, mipLevels, arrayLayers, sampleFlags, vk::ImageTiling::eOptimal, imageUsageFlags, vk::MemoryPropertyFlagBits::eDeviceLocal) {

		if (data != nullptr) {
			VulkanBuffer stagingBuffer(data, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
			
			vk::ImageAspectFlags flags = VulkanDimResource::getImageAspectFlags(imageUsageFlags);

			changeLayout(Rendering::QueueType::TRANSFER, vk::ImageLayout::eTransferDstOptimal, { flags, 0, mipLevels, 0, arrayLayers });
			copyFrom(stagingBuffer, { { 0, 0, 0, {flags, 0, 0, arrayLayers}, {}, extent } });

		}
	}

}


