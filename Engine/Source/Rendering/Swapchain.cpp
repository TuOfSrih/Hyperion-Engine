#include "Swapchain.hpp"

#include "RenderContext.hpp"

#include <algorithm>


namespace Hyperion::Rendering {

	Swapchain::Swapchain(const Configuration& config, VideoSettings& videoSettings, const QueueFamilyIndices& queueIndices)
	{
		const vk::Device& device = RenderContext::active->getDevice();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(videoSettings.resolution.width, videoSettings.resolution.height, config.applicationName.c_str(), NULL, NULL);
		VkSurfaceKHR surf;
		//TODO ASSERT
		glfwCreateWindowSurface(RenderContext::active->getInstance() , window, nullptr, &surf);
		surface = surf;

		//TODO Check all Queue families? 
		RenderContext::active->getGPU().getSurfaceSupportKHR(queueIndices.graphicsIndex, surface);

		vk::SurfaceFormatKHR surfaceFormat{};
		vk::PresentModeKHR presentMode;

		vk::SurfaceCapabilitiesKHR surfaceCapabilites = RenderContext::active->getGPU().getSurfaceCapabilitiesKHR(surface);
		if (surfaceCapabilites.maxImageCount && surfaceCapabilites.maxImageCount < videoSettings.bufferImageCount)
			videoSettings.bufferImageCount = static_cast<uint8_t>(surfaceCapabilites.maxImageCount);

		videoSettings.resolution = {
			std::clamp(videoSettings.resolution.width, surfaceCapabilites.minImageExtent.width, surfaceCapabilites.maxImageExtent.width),
			std::clamp(videoSettings.resolution.height, surfaceCapabilites.minImageExtent.height, surfaceCapabilites.maxImageExtent.height)
		};

		//TODO Check whether something wasnt checked to be available and general digging into formats + colorspaces
		std::vector<vk::SurfaceFormatKHR> surfaceFormats{ RenderContext::active->getGPU().getSurfaceFormatsKHR(surface) };
		for (auto& format : surfaceFormats) {

			//TODO Get more into colour formats for surfaces and other rendertargets
			if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				surfaceFormat = format;
		}
		//TODO What if not found?

		std::vector<vk::PresentModeKHR> presentModes{ RenderContext::active->getGPU().getSurfacePresentModesKHR(surface) };
		if (std::find(presentModes.cbegin(), presentModes.cend(), vk::PresentModeKHR::eMailbox) == presentModes.end()) {

			presentMode = vk::PresentModeKHR::eFifo;
		}
		else {

			presentMode = vk::PresentModeKHR::eMailbox;
		}

		swapchain = device.createSwapchainKHR(
			vk::SwapchainCreateInfoKHR(
				{},
				surface,
				videoSettings.bufferImageCount,
				surfaceFormat.format,
				surfaceFormat.colorSpace,
				videoSettings.resolution,
				1,
				vk::ImageUsageFlagBits::eColorAttachment, //TODO Check whether correct
				vk::SharingMode::eExclusive,
				0,
				nullptr,
				vk::SurfaceTransformFlagBitsKHR::eIdentity,
				vk::CompositeAlphaFlagBitsKHR::eOpaque,
				presentMode,
				VK_TRUE,	// TODO Check, whether this option is unproblematic
				swapchain
			)
		);
		swapchainImageViews.reserve(videoSettings.bufferImageCount);
		auto swapChainImages = device.getSwapchainImagesKHR(swapchain);
		for (auto& image : swapChainImages) {

			vk::ImageViewCreateInfo imageViewInfo{
				{},
				image,
				vk::ImageViewType::e2D,
				surfaceFormat.format,
				{},
				vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
			};

			swapchainImageViews.push_back(device.createImageView(imageViewInfo));
		}
	}

	Swapchain::Swapchain(Swapchain&& other) {
		*this = std::move(other);
	}

	Swapchain& Swapchain::operator=(Swapchain&& other) {

		if (this != &other) {

			window = std::move(other.window);
			surface = std::move(other.surface);
			swapchain = std::move(other.swapchain);
			swapchainImageViews = std::move(other.swapchainImageViews);

			other.window = nullptr;
			other.surface = vk::SurfaceKHR();
			other.swapchain = vk::SwapchainKHR();
			other.swapchainImageViews.clear();

		}
		return *this;
	}
	Swapchain::~Swapchain()
	{
		const vk::Device& device = RenderContext::active->getDevice();
		const vk::Instance& instance = RenderContext::active->getInstance();
		for (auto& imageView : swapchainImageViews) device.destroyImageView(imageView);
		device.destroySwapchainKHR(swapchain);
		instance.destroySurfaceKHR(surface);

		glfwDestroyWindow(window);
	}
	const vk::SwapchainKHR& Swapchain::getRaw() const
	{
		return swapchain;
	}
	const vk::ImageView& Swapchain::getImageView(size_t bufferIndex) const
	{
		return swapchainImageViews[bufferIndex];
	}
}