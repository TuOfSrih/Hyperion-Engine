#pragma once

#include <vector>

#include "vulkan/vulkan.hpp"

#include "System/Configuration.hpp"


namespace Hyperion::Rendering {

	class RenderContext {
	public:
		RenderContext(const Configuration& config);

	private:
		const std::vector<const char*> instanceLayers = {
#ifdef _DEBUG
			"VK_LAYER_LUNARG_standard_validation"
#endif
		};
		//TODO Find a way to make it const as well
		std::vector<const char*> instanceExtensions = {
#ifdef _DEBUG
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
		};
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		vk::Instance instance;
		vk::PhysicalDevice phyDevice;
		vk::Device device;
		vk::SwapchainKHR swapChain;

	};
}