#pragma once

#include "vulkan/vulkan.hpp"

#include <inttypes.h>


namespace Hyperion::Rendering {

	class VideoSettings {

	public:
		static constexpr uint8_t maxBufferImageCount = 3;

		vk::Extent2D resolution{ 1920, 1080 };
		uint8_t bufferImageCount = 3;
	};
}