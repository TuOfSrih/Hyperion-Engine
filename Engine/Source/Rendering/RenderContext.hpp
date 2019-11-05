#pragma once

#include <vector>

#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

#include "System/Configuration.hpp"
#include "Rendering/VideoSettings.hpp"
#include "Rendering/Pipeline.hpp"


namespace Hyperion::Rendering {

	//TODO Subdivide into Context + VulkanContext
	class QueueFamilyIndices {
	public:
		const uint32_t graphicsIndex;
		const uint32_t transferIndex;
		const uint32_t computeIndex;
	};

	class RenderContext {
	public:
		explicit RenderContext(const Configuration& config);
		~RenderContext();

		const vk::Device& getDevice();
		const VideoSettings& getVideoSettings();

		
		static RenderContext* active;
		void setContext(RenderContext* newContext);
		

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
		vk::Queue graphicsQueue;
		vk::Queue computeQueue;
		vk::Queue transferQueue;
		vk::SurfaceKHR surface;
		//TODO Separate class for swapchain?
		vk::SwapchainKHR swapchain{};
		std::vector<vk::ImageView> swapchainImageViews{};
		std::vector<vk::CommandPool> graphicsCmdPools{};
		vk::CommandPool transferCmdPool;
		vk::CommandPool computeCmdPool;
		PipelineHandler pipelineHandler;

		VideoSettings videoSettings;

		vk::PhysicalDevice pickGPU();
		std::vector<vk::DeviceQueueCreateInfo> getQueueCreateInfo(const std::vector<float>& prios);
		vk::PhysicalDeviceFeatures getDeviceFeatures();
		QueueFamilyIndices getQueueFamilyIndices();

	};

}