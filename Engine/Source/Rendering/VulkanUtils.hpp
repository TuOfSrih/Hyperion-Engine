#pragma once

#include <vector>
#include <functional>

#include "vulkan/vulkan.hpp"

#include "Queue.hpp"

namespace Hyperion::Rendering::Vulkan {

	class SharingInfo {
	public:

		explicit SharingInfo(const std::vector<uint32_t>& indices = {});

		bool isShared() const;
		vk::SharingMode getSharingMode() const;
		
		const uint32_t* getIndices() const;
		size_t getAmount() const;

	private:
		std::vector<uint32_t> queueIndices;
	};

	vk::CommandBuffer simpleExecute(const QueueTypeInfo& queueInfo, const std::function<void(const vk::CommandBuffer& cmdBuffer)>& func);
	vk::CommandBuffer simpleExecuteGraphics(const std::function<void(const vk::CommandBuffer& cmdBuffer)>& func);
	vk::CommandBuffer simpleExecuteCompute(const std::function<void(const vk::CommandBuffer& cmdBuffer)>& func);
	vk::CommandBuffer simpleExecuteTransfer(const std::function<void(const vk::CommandBuffer& cmdBuffer)>& func);
}