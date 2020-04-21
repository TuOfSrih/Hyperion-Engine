#pragma once

#include "vulkan/vulkan.hpp"

#include <inttypes.h>


namespace Hyperion::Rendering {

	enum class QueueType : uint8_t {
		INVALID = 0,
		GRAPHICS,
		COMPUTE,
		TRANSFER
	};

	class QueueTypeInfo {
	public:
		QueueTypeInfo() : type(QueueType::INVALID) {};
		QueueTypeInfo(const QueueType& type) : type(type) {};

		const vk::Queue& getQueue() const;
		const vk::CommandPool& getCommandPool() const;

	private:
		QueueType type;
	};

	class QueueFamilyIndices {
	public:
		const uint32_t graphicsIndex;
		const uint32_t transferIndex;
		const uint32_t computeIndex;
	};
}