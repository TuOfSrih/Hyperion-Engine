#include "CommandPoolController.hpp"

#include "Rendering/RenderContext.hpp"

namespace Hyperion::System::Memory {

	CommandPoolController::CommandPoolController(const int threadCount, const Rendering::QueueFamilyIndices& queueFamilyIndices)
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		int totalPools = threadCount * Rendering::RenderContext::active->getVideoSettings().bufferImageCount;
		graphicsPools.reserve(totalPools);
		for (int i = 0; i < totalPools; ++i) {

			device.createCommandPool(
				vk::CommandPoolCreateInfo{
					{},
					queueFamilyIndices.graphicsIndex
				}
			);
		}

		transferPool = device.createCommandPool(
			vk::CommandPoolCreateInfo{
				{},
				queueFamilyIndices.transferIndex
			}
		);

		computePool = device.createCommandPool(
			vk::CommandPoolCreateInfo{
				{},
				queueFamilyIndices.computeIndex
			}
		);

	}
	CommandPoolController::~CommandPoolController()
	{
		const vk::Device& device = Rendering::RenderContext::active->getDevice();
		device.destroyCommandPool(computePool);
		device.destroyCommandPool(transferPool);
		for (auto& pool : graphicsPools) device.destroyCommandPool(pool);
	}
	const vk::CommandPool& CommandPoolController::getGraphicsPool(const int threadID, const int bufferImageIndex) const
	{
		return graphicsPools.at((threadID)*Rendering::RenderContext::active->getVideoSettings().bufferImageCount + bufferImageIndex);
	}

	const vk::CommandPool& CommandPoolController::getTransferPool() const
	{
		return transferPool;
	}
	const vk::CommandPool& CommandPoolController::getComputePool() const
	{
		return computePool;
	}
}