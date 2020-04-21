#pragma once

#include "vulkan/vulkan.hpp"

#include <inttypes.h>


namespace Hyperion::Rendering {

	class VideoSettings {

	public:
		vk::Extent2D resolution{ 1920, 1080 };
		uint8_t bufferImageCount = 3;
	};
}