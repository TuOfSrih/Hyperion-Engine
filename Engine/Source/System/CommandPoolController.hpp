#pragma once

#include "Rendering/Queue.hpp"
#include "Debug.hpp"

namespace Hyperion::System::Memory {


	class CommandPoolController {
	public:
		CommandPoolController() = default;
		CommandPoolController(const int threadCount, const Rendering::QueueFamilyIndices& queueFamilyIndices);
		~CommandPoolController();
		noCopy(CommandPoolController);
		declMove(CommandPoolController);
		

		const vk::CommandPool& getGraphicsPool(const int threadID, const int bufferImageIndex) const;
		const vk::CommandPool& getTransferPool() const;
		const vk::CommandPool& getComputePool() const;

	private:
		std::vector<vk::CommandPool> graphicsPools;
		vk::CommandPool transferPool;
		vk::CommandPool computePool;
	};
}