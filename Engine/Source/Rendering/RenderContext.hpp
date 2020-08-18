#pragma once

#include "Queue.hpp"
#include "Swapchain.hpp"
#include "VideoSettings.hpp"
#include "Pipeline.hpp"
#include "DrawController.hpp"


#include "System/VulkanPoolController.hpp"
#include "System/Configuration.hpp"
#include "System/Debug.hpp"
#include "System/Memory.hpp"
#include "System/Thread.hpp"

#include "vulkan/vulkan.hpp"

#include <vector>


namespace Hyperion::Rendering {

	const vk::FrontFace RenderOrder = vk::FrontFace::eClockwise;//Collect somewhere? TODO

	//TODO Subdivide into Context + VulkanContext
	class RenderContext {
	public:
		explicit RenderContext(const Configuration& config);
		~RenderContext();

		const vk::Device& getDevice() const;
		const vk::Instance& getInstance() const;
		const vk::PhysicalDevice& getGPU() const;
		const VideoSettings& getVideoSettings() const;

		const vk::Queue& getGraphicsQueue() const;
		const vk::Queue& getComputeQueue() const;
		const vk::Queue& getTransferQueue() const;

		const QueueFamilyIndices getQueueFamilyIndices() const;

		const vk::CommandPool& getActiveGraphicsPool() const;
		const vk::CommandPool& getGraphicsPool(const int threadID, const int bufferImageIndex) const;
		const vk::CommandPool& getTransferPool() const;
		const vk::CommandPool& getComputePool() const;

		const Swapchain& getSwapchain() const;
		uint32_t getActiveBufferIndex() const;

		const PipelineHandler& getPipelineHandler() const;
		
		static const RenderContext* active;
		void setContext(RenderContext* newContext);

		void render();
		

	private:

		vk::PhysicalDevice pickGPU();
		void checkLayerSupport();
		void checkInstanceExtensionSupport();
		void checkDeviceExtensionSupport();
		std::vector<vk::DeviceQueueCreateInfo> getQueueCreateInfo(const std::vector<float>& prios);
		vk::PhysicalDeviceFeatures getDeviceFeatures();


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
		VideoSettings videoSettings;

		vk::Instance instance;
#ifdef _DEBUG
		Debug::VulkanTools debugTools;
#endif
		vk::PhysicalDevice gpu;
		vk::Device device;

		vk::Queue graphicsQueue;
		vk::Queue computeQueue;
		vk::Queue transferQueue;

		Swapchain swapchain;
		uint32_t activeBufferIndex;
		System::Memory::VulkanPoolController cmdPoolController;

		DrawController drawController;
		PipelineHandler pipelineHandler;

		std::vector<vk::Fence> bufferingFences;
		std::vector<vk::Semaphore> resourcesAvailableSemaphores;
		std::vector<vk::Semaphore> frameFinishedSemaphores;
		
	};

}