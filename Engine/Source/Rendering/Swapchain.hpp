#pragma once

#include "VideoSettings.hpp"
#include "Queue.hpp"

#include "System/Configuration.hpp"
#include "Cpp/AdvancedOperators.hpp"

#include "GLFW/glfw3.h"


namespace Hyperion::Rendering {

	class Swapchain {
	public:

		Swapchain() = default;
		Swapchain(const Configuration& config, VideoSettings& videoSettings, const QueueFamilyIndices& queueIndices);
		noCopy(Swapchain);
		declMove(Swapchain);
		~Swapchain();

		const vk::SwapchainKHR& getRaw() const;

	private:
		GLFWwindow* window;
		vk::SurfaceKHR surface;
		vk::SwapchainKHR swapchain{};
		std::vector<vk::ImageView> swapchainImageViews{};
	};
}