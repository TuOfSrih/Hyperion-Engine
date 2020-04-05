#pragma once

#include <vector>

#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

#include "Queue.hpp"
#include "System/Configuration.hpp"
#include "Rendering/VideoSettings.hpp"
#include "Rendering/Pipeline.hpp"
#include "System/Debug.hpp"
#include "System/Memory.hpp"
#include "System/CommandPoolController.hpp"


namespace Hyperion::Rendering {

	//TODO Subdivide into Context + VulkanContext
	class RenderContext {
	public:
		explicit RenderContext(const Configuration& config);
		~RenderContext();

		const vk::Device& getDevice() const ;
		const vk::PhysicalDevice& getGPU() const;
		const VideoSettings& getVideoSettings() const;

		const vk::Queue& getGraphicsQueue() const;
		const vk::Queue& getComputeQueue() const;
		const vk::Queue& getTransferQueue() const;

		const QueueFamilyIndices getQueueFamilyIndices() const;

		const vk::CommandPool& getGraphicsPool(const int threadID, const int bufferImageIndex) const;
		const vk::CommandPool& getTransferPool() const;
		const vk::CommandPool& getComputePool() const;

		
		static const RenderContext* active;
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
		vk::PhysicalDevice gpu;
		vk::Device device;

		vk::Queue graphicsQueue;
		vk::Queue computeQueue;
		vk::Queue transferQueue;
		vk::SurfaceKHR surface;
		//TODO Separate class for swapchain?
		vk::SwapchainKHR swapchain{};
		std::vector<vk::ImageView> swapchainImageViews{};

		System::Memory::CommandPoolController cmdPoolController;

#ifdef _DEBUG
		Debug::VulkanTools debugTools;
#endif

		PipelineHandler pipelineHandler;

		VideoSettings videoSettings;

		vk::PhysicalDevice pickGPU();
		void checkLayerSupport();
		void checkInstanceExtensionSupport();
		void checkDeviceExtensionSupport();
		std::vector<vk::DeviceQueueCreateInfo> getQueueCreateInfo(const std::vector<float>& prios);
		vk::PhysicalDeviceFeatures getDeviceFeatures();

	};

}