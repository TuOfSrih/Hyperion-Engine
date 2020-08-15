#pragma once

#include "Debug.hpp"
#include "Rendering/Queue.hpp"

#include <vector>


namespace Hyperion::System::Memory {


	class VulkanPoolController {
	public:
		VulkanPoolController() = default;
		VulkanPoolController(const int threadCount, const Rendering::QueueFamilyIndices& queueFamilyIndices);
		~VulkanPoolController();
		noCopy(VulkanPoolController);
		declMove(VulkanPoolController);
		

		const vk::CommandPool& getGraphicsPool(const int threadID, const int bufferImageIndex) const;
		const vk::CommandPool& getTransferPool() const;
		const vk::CommandPool& getComputePool() const;

	private:
		std::vector<vk::CommandPool> graphicsPools;
		vk::CommandPool transferPool;
		vk::CommandPool computePool;

		vk::DescriptorPool descriptorPool;//Proper handling of descriptorPools + sets
	};
}