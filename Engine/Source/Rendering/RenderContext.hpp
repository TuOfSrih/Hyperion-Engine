#pragma once

#include <vector>

#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

#include "System/Configuration.hpp"


namespace Hyperion::Rendering {

	class RenderContext {
	public:
		explicit RenderContext(const Configuration& config);

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
		GLFWwindow* window;
		vk::PhysicalDevice physDevice;
		vk::Device device;

		vk::Extent2D resolution = { 1920, 1080 };

		vk::PhysicalDevice pickGPU();
		std::vector<vk::DeviceQueueCreateInfo> getQueueCreateInfo();
		vk::PhysicalDeviceFeatures getDeviceFeatures();

	};
}