#pragma once

#include <inttypes.h>

namespace Hyperion::Rendering {

	class QueueFamilyIndices {
	public:
		const uint32_t graphicsIndex;
		const uint32_t transferIndex;
		const uint32_t computeIndex;
	};
}