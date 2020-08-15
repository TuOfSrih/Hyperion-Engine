
#include "VulkanPoolController.hpp"

#include "Rendering/RenderContext.hpp"
#include "Rendering/Queue.hpp"


namespace Hyperion::System::Memory {

	VulkanPoolController::VulkanPoolController(const int threadCount, const Rendering::QueueFamilyIndices& queueFamilyIndices)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		int totalPools = threadCount * Rendering::RenderContext::active->getVideoSettings().bufferImageCount;
		graphicsPools.reserve(totalPools);
		for (int i = 0; i < totalPools; ++i) {

			graphicsPools.emplace_back(
				device.createCommandPool(
					vk::CommandPoolCreateInfo{
						{},
						queueFamilyIndices.graphicsIndex
					}
				)
			);
		}

		transferPool = device.createCommandPool(
			vk::CommandPoolCreateInfo{
				{},//vk::CommandPoolCreateFlagBits::eTransient,
				queueFamilyIndices.transferIndex
			}
		);

		computePool = device.createCommandPool(
			vk::CommandPoolCreateInfo{
				{},
				queueFamilyIndices.computeIndex
			}
		);
		uint32_t bufferImageCount = Rendering::RenderContext::active->getVideoSettings().bufferImageCount;
		std::vector<vk::DescriptorPoolSize> poolSizes = {
			{vk::DescriptorType::eUniformBuffer, bufferImageCount}
		};

		descriptorPool = device.createDescriptorPool({
			{},
			bufferImageCount,
			static_cast<uint32_t>(poolSizes.size()),
			poolSizes.data()
		});

	}
	VulkanPoolController::~VulkanPoolController()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();

		device.destroyDescriptorPool(descriptorPool);

		device.destroyCommandPool(computePool);
		device.destroyCommandPool(transferPool);
		for (auto& pool : graphicsPools) device.destroyCommandPool(pool);
	}
	VulkanPoolController::VulkanPoolController(VulkanPoolController&& other)
	{
		*this = std::move(other);
	}
	VulkanPoolController& VulkanPoolController::operator=(VulkanPoolController&& other)
	{
		this->~VulkanPoolController();

		if (this != &other) {

			this->graphicsPools = std::move(other.graphicsPools);
			this->transferPool = std::move(other.transferPool);
			this->computePool = std::move(other.computePool);

			other.graphicsPools.clear();
			other.transferPool = vk::CommandPool();
			other.computePool = vk::CommandPool();
		}

		return *this;
	}
	const vk::CommandPool& VulkanPoolController::getGraphicsPool(const int threadID, const int bufferImageIndex) const
	{
		return graphicsPools.at((threadID)*Rendering::RenderContext::active->getVideoSettings().bufferImageCount + bufferImageIndex);
	}

	const vk::CommandPool& VulkanPoolController::getTransferPool() const
	{
		return transferPool;
	}
	const vk::CommandPool& VulkanPoolController::getComputePool() const
	{
		return computePool;
	}
}