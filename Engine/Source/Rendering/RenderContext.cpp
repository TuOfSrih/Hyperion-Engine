#include "RenderContext.hpp"

#include "Math/Utils.hpp"
#include "System/Debug.hpp"

namespace Hyperion::Rendering {

	//TODO Subdivide Function into multiple
	Hyperion::Rendering::RenderContext::RenderContext(const Configuration& config)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(videoSettings.resolution.width, videoSettings.resolution.height, config.applicationName.c_str(), NULL, NULL);

		uint32_t count;
		const char** glfwExt = glfwGetRequiredInstanceExtensions(&count);
		instanceExtensions.insert(instanceExtensions.end(), glfwExt, glfwExt + count);

		const vk::ApplicationInfo appInfo(
			config.applicationName.c_str(),
			config.applicationVersion.getRaw(),
			config.engineName.c_str(),
			config.engineVersion.getRaw(),
			VK_MAKE_VERSION(1, 1, 121)
		);

		instance = vk::createInstance(
			vk::InstanceCreateInfo(
				{}, &appInfo,
				//TODO better solution than casts?
				static_cast<uint32_t>(instanceLayers.size()),
				instanceLayers.data(),
				static_cast<uint32_t>(instanceExtensions.size()),
				instanceExtensions.data()
			)
		);
		VkSurfaceKHR surf;
		//TODO ASSERT
		glfwCreateWindowSurface(instance, window, nullptr, &surf);
		surface = surf;
#ifdef _DEBUG
		Debug::setUpVulkanDebugging(instance);
#endif
		

		physDevice = pickGPU();
		//TODO Source of no portability, find better solution for finding out how many familites
		std::vector<float> prios(physDevice.getQueueFamilyProperties().size());
		std::fill(prios.begin(), prios.end(), 1.0f);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = getQueueCreateInfo(prios);
		vk::PhysicalDeviceFeatures deviceFeatures = getDeviceFeatures();
		device = physDevice.createDevice(
			vk::DeviceCreateInfo(
				vk::DeviceCreateFlags(),
				static_cast<uint32_t>(queueCreateInfos.size()),
				queueCreateInfos.data(),
				0,
				nullptr,
				static_cast<uint32_t>(deviceExtensions.size()),
				deviceExtensions.data(),
				&deviceFeatures
			)
		);

		const QueueFamilyIndices indices = getQueueFamilyIndices();
		graphicsQueue = device.getQueue(indices.graphicsIndex, 0);
		transferQueue = device.getQueue(indices.transferIndex, indices.transferIndex == indices.graphicsIndex);
		computeQueue = device.getQueue(indices.computeIndex, (indices.computeIndex == indices.graphicsIndex) + (indices.computeIndex == indices.transferIndex));

		//TODO Check all Queue families? 
		physDevice.getSurfaceSupportKHR(indices.graphicsIndex, surface);

		vk::SurfaceFormatKHR surfaceFormat{};
		vk::PresentModeKHR presentMode;

		vk::SurfaceCapabilitiesKHR surfaceCapabilites = physDevice.getSurfaceCapabilitiesKHR(surface);
		if (surfaceCapabilites.maxImageCount && surfaceCapabilites.maxImageCount > videoSettings.imageCount)
			videoSettings.imageCount = static_cast<uint8_t>(surfaceCapabilites.maxImageCount);

		//TODO Check whether something wasnt checked to be available and general digging into formats + colorspaces
		std::vector<vk::SurfaceFormatKHR> surfaceFormats{ physDevice.getSurfaceFormatsKHR(surface) };
		for (auto& format : surfaceFormats) {

			if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				surfaceFormat = format;
		}
		//What if not found?

		std::vector<vk::PresentModeKHR> presentModes{ physDevice.getSurfacePresentModesKHR(surface)};
		if (std::find(presentModes.cbegin(), presentModes.cend(), vk::PresentModeKHR::eMailbox) == presentModes.end()) {
			
			presentMode = vk::PresentModeKHR::eFifo;
		}
		else {

			presentMode = vk::PresentModeKHR::eMailbox;
		}

		videoSettings.resolution = { std::clamp(videoSettings.resolution.width, surfaceCapabilites.minImageExtent.width, surfaceCapabilites.maxImageExtent.width),
			std::clamp(videoSettings.resolution.height, surfaceCapabilites.minImageExtent.height, surfaceCapabilites.maxImageExtent.height) 
		};

		swapchain = device.createSwapchainKHR(
			vk::SwapchainCreateInfoKHR(
				{},
				surface,
				videoSettings.imageCount,
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

	}
	
	RenderContext::~RenderContext()
	{
		device.destroySwapchainKHR(swapchain);
		device.destroy();
#ifdef _DEBUG
		Debug::cleanUpVulkanDebugging(instance);
#endif
		
		instance.destroySurfaceKHR(surface);
		instance.destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
		
	}

	//TODO More sophisticated process, but how? Most VRAM? All comparison operators?
	bool operator>(const vk::PhysicalDeviceProperties& leftProps, const vk::PhysicalDeviceProperties& rightProps) {

		if (leftProps.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
			&& rightProps.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
			return true;

		if (leftProps.deviceType != vk::PhysicalDeviceType::eDiscreteGpu
			&& rightProps.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			return false;

		return leftProps.limits.maxViewportDimensions[0] * leftProps.limits.maxViewportDimensions[1] >=
			rightProps.limits.maxViewportDimensions[0] * rightProps.limits.maxViewportDimensions[1];
	}

	//Check whether extensions are supported
	vk::PhysicalDevice RenderContext::pickGPU()
	{
		const std::vector<vk::PhysicalDevice> gpus = instance.enumeratePhysicalDevices();
		vk::PhysicalDevice bestGPU = gpus.at(0);
		vk::PhysicalDeviceProperties bestProps = bestGPU.getProperties();

		for (int i = 1; i < gpus.size(); ++i) {
			vk::PhysicalDeviceProperties gpuProps = gpus[i].getProperties();
			if (gpuProps > bestProps) {

				bestGPU = gpus[i];
				bestProps = gpuProps;
			}
		}

		return bestGPU;
	}

	std::vector<vk::DeviceQueueCreateInfo> RenderContext::getQueueCreateInfo(const std::vector<float>& prios)
	{
		const QueueFamilyIndices indices{ getQueueFamilyIndices() };
		//Avoid magic number for queue amounts
		std::vector<vk::DeviceQueueCreateInfo> queueInfos;
		uint32_t identicalToGraphics = (indices.graphicsIndex == indices.transferIndex) + (indices.graphicsIndex == indices.computeIndex);
		queueInfos.emplace_back(vk::DeviceQueueCreateInfo{ {}, indices.graphicsIndex, 1 + identicalToGraphics, prios.data() });

		//Add new QueueCreateInfo if not already in previous info
		if (indices.transferIndex != indices.graphicsIndex) {

			uint32_t identicalToTransfer = indices.computeIndex == indices.transferIndex;
			queueInfos.emplace_back(vk::DeviceQueueCreateInfo{ {}, indices.transferIndex, 1 + identicalToTransfer, prios.data() });
		}

		//Add new QueueCreateInfo if not already in previous infos
		if( indices.computeIndex != indices.graphicsIndex && indices.computeIndex != indices.transferIndex)
			queueInfos.emplace_back(vk::DeviceQueueCreateInfo{ {}, indices.computeIndex, 1, prios.data() });
			

		return queueInfos;
	}

	vk::PhysicalDeviceFeatures RenderContext::getDeviceFeatures()
	{
		//TODO Revisit later and check whether everything is included
		VkPhysicalDeviceFeatures features = {};
#ifdef _DEBUG
		features.robustBufferAccess = VK_TRUE;
#endif
		features.geometryShader = VK_TRUE;
		features.tessellationShader = VK_TRUE;
		features.depthClamp = VK_TRUE;
		features.depthBiasClamp = VK_TRUE;
		features.fillModeNonSolid = VK_TRUE;
		features.depthBounds = VK_TRUE;
		/*features.wideLines = VK_TRUE;
		features.largePoints = VK_TRUE;
		features.multiViewport = VK_TRUE*/
		features.samplerAnisotropy = VK_TRUE;
		/*features.occlusionQueryPrecise = VK_TRUE;
		features.vertexPipelineStoresAndAtomics = VK_TRUE;
		features.fragmentStoresAndAtomics = VK_TRUE;
		features.shaderTessellationAndGeometryPointSize = VK_TRUE;
		features.shaderStorageImageMultisample = VK_TRUE;
		features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
		features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
		features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
		features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
		features.shaderClipDistance = VK_TRUE;
		features.shaderCullDistance = VK_TRUE;
		features.shaderInt16 = VK_TRUE;
		features.shaderResourceMinLod = VK_TRUE;
		features.sparseBinding = VK_TRUE;*/


		return vk::PhysicalDeviceFeatures(features);
	}

	QueueFamilyIndices RenderContext::getQueueFamilyIndices()
	{
		std::vector<vk::QueueFamilyProperties> props = physDevice.getQueueFamilyProperties();
		uint32_t graphics, transfer, compute;
		graphics = transfer = compute = std::numeric_limits<uint32_t>::max();
		int maxDist = std::numeric_limits<int>::max();

		//Get graphics index
		for (uint32_t i = 0; i < props.size(); ++i) {
			
			if (props.at(i).queueFlags & vk::QueueFlags(VK_QUEUE_GRAPHICS_BIT)) {

				const int curDist = Math::hammingDistance(static_cast<unsigned int>(props.at(i).queueFlags), VK_QUEUE_GRAPHICS_BIT);
				//TODO Avoid magic number for maximum amount of enum different bits
				if (curDist < 5 && curDist < maxDist) {

					maxDist = curDist;
					graphics = i;
				}
			}
		}

		//Get transfer index
		maxDist = std::numeric_limits<int>::max();
		for (uint32_t i = 0; i < props.size(); ++i) {

			if (props.at(i).queueFlags & vk::QueueFlags(VK_QUEUE_TRANSFER_BIT)) {

				const int curDist = Math::hammingDistance(static_cast<unsigned int>(props.at(i).queueFlags), VK_QUEUE_TRANSFER_BIT);
				//TODO Avoid magic number for maximum amount of enum different bits
				if (curDist < 5 &&
					curDist < maxDist && 
					(i != graphics || props.at(i).queueCount > 1)) {

					maxDist = curDist;
					transfer = i;
				}
			}
		}

		//Get compute index
		maxDist = std::numeric_limits<int>::max();
		for (uint32_t i = 0; i < props.size(); ++i) {

			if (props.at(i).queueFlags & vk::QueueFlags(VK_QUEUE_COMPUTE_BIT)) {

				const int curDist = Math::hammingDistance(static_cast<unsigned int>(props.at(i).queueFlags), VK_QUEUE_COMPUTE_BIT);
				//TODO Avoid magic number for maximum amount of enum different bits
				if (curDist < 5 && curDist < maxDist)  {

					const int bitCount = (i == graphics) + (i == transfer);
					if (props.at(i).queueCount > 0) {

						maxDist = curDist;
						compute = i;
					}
					
				}
			}
		}


		
		return { graphics, transfer, compute };
	}
}




