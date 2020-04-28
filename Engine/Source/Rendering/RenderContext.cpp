#include "RenderContext.hpp"

#include "Math/BitOperations.hpp"
#include "System/Debug.hpp"

#include <thread>

namespace Hyperion::Rendering {

	const RenderContext* RenderContext::active = nullptr;

	//TODO Subdivide Function into multiple
	Hyperion::Rendering::RenderContext::RenderContext(const Configuration& config)
	{
		setContext(this);
		//assert
		glfwInit();
		

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

		checkLayerSupport();

		instance = vk::createInstance(
			vk::InstanceCreateInfo(
				{}, 
				&appInfo,
				static_cast<uint32_t>(instanceLayers.size()),
				instanceLayers.data(),
				static_cast<uint32_t>(instanceExtensions.size()),
				instanceExtensions.data()
			)
		);
		
#ifdef _DEBUG
		debugTools = Debug::VulkanTools(instance);
#endif
		

		gpu = pickGPU();
		//TODO Source of no portability, find better solution for finding out how many familites
		std::vector<float> prios(gpu.getQueueFamilyProperties().size());
		std::fill(prios.begin(), prios.end(), 1.0f);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = getQueueCreateInfo(prios);
		vk::PhysicalDeviceFeatures deviceFeatures = getDeviceFeatures();

		checkDeviceExtensionSupport();
		device = gpu.createDevice(
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

		const QueueFamilyIndices queueIndices = getQueueFamilyIndices();
		graphicsQueue = device.getQueue(queueIndices.graphicsIndex, 0);
		transferQueue = device.getQueue(queueIndices.transferIndex, queueIndices.transferIndex == queueIndices.graphicsIndex);
		computeQueue = device.getQueue(queueIndices.computeIndex, (queueIndices.computeIndex == queueIndices.graphicsIndex) + (queueIndices.computeIndex == queueIndices.transferIndex));

		swapchain = Swapchain(config, videoSettings, queueIndices);
		
		cmdPoolController = System::Memory::CommandPoolController(std::thread::hardware_concurrency() + 1, queueIndices);
		pipelineHandler = PipelineHandler(config);
	}
	
	RenderContext::~RenderContext()
	{
		pipelineHandler.~PipelineHandler();
		cmdPoolController.~CommandPoolController();
		swapchain.~Swapchain();
		device.destroy();
#ifdef _DEBUG
		debugTools.~VulkanTools();
#endif
		
		
		instance.destroy();
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

	//TODO Review C++ bindings style
	void RenderContext::checkLayerSupport()
	{
		uint32_t amountSupportedValidationLayers = 0;
		vkEnumerateInstanceLayerProperties(&amountSupportedValidationLayers, nullptr);

		std::vector<VkLayerProperties> supportedValidationLayerProperties(amountSupportedValidationLayers);
		vkEnumerateInstanceLayerProperties(&amountSupportedValidationLayers, supportedValidationLayerProperties.data());


		for (const char* layer : instanceLayers) {

			bool found = false;
			for (const VkLayerProperties layerProperties : supportedValidationLayerProperties) {

				if (!strcmp(layer, layerProperties.layerName)) {

					found = true;
					break;
				}
			}
			if (!found) {

				Debug::missingSupport("An extension was not supported!");
			}
		}
	}

	void RenderContext::checkInstanceExtensionSupport()
	{
		uint32_t amountSupportedExtensions = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &amountSupportedExtensions, nullptr);

		std::vector<VkExtensionProperties> supportedExtensionProperties(amountSupportedExtensions);
		vkEnumerateInstanceExtensionProperties(nullptr, &amountSupportedExtensions, supportedExtensionProperties.data());

		for (const char* extension : instanceExtensions) {

			bool found = false;
			for (const VkExtensionProperties extensionProperties : supportedExtensionProperties) {

				if (!strcmp(extension, extensionProperties.extensionName)) {

					found = true;
					break;
				}
			}
			if (!found) {

				Debug::missingSupport("An extension was not supported!");
			}
		}
	}

	void RenderContext::checkDeviceExtensionSupport()
	{
		uint32_t amountSupportedDeviceExtensions = 0;
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &amountSupportedDeviceExtensions, nullptr);

		std::vector<VkExtensionProperties>supportedExtensionProperties(amountSupportedDeviceExtensions);
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &amountSupportedDeviceExtensions, supportedExtensionProperties.data());

		for (const char* extension : deviceExtensions) {

			bool found = false;
			for (const VkExtensionProperties extensionProperties : supportedExtensionProperties) {

				if (!strcmp(extension, extensionProperties.extensionName)) {

					found = true;
					break;
				}
			}
			if (!found) {

				Debug::missingSupport("An extension was not supported!");
			}
		}
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

	const QueueFamilyIndices RenderContext::getQueueFamilyIndices() const
	{
		std::vector<vk::QueueFamilyProperties> props = gpu.getQueueFamilyProperties();
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
	const vk::CommandPool& RenderContext::getCurrentGraphicsPool() const
	{
		return getGraphicsPool(System::Thread::threadNum, swapchain.getBufferImageIndex());
	}
	const vk::CommandPool& RenderContext::getGraphicsPool(const int threadID, const int bufferImageIndex) const
	{
		return cmdPoolController.getGraphicsPool(threadID, bufferImageIndex);
	}
	const vk::CommandPool& RenderContext::getTransferPool() const
	{
		return cmdPoolController.getTransferPool();
	}
	const vk::CommandPool& RenderContext::getComputePool() const
	{
		return cmdPoolController.getComputePool();
	}
	const vk::Device & RenderContext::getDevice() const 
	{
		return device;
	}
	const vk::Instance& RenderContext::getInstance() const
	{
		return instance;
	}
	const vk::PhysicalDevice& RenderContext::getGPU() const
	{
		return gpu;
	}
	const VideoSettings & RenderContext::getVideoSettings() const
	{
		return videoSettings;
	}
	const vk::Queue& RenderContext::getGraphicsQueue() const
	{
		return graphicsQueue;
	}
	const vk::Queue& RenderContext::getComputeQueue() const
	{
		return computeQueue;
	}
	const vk::Queue& RenderContext::getTransferQueue() const
	{
		return transferQueue;
	}
	void RenderContext::setContext(RenderContext * newContext)
	{
		ASSERT(active == nullptr);
		active = newContext;
	}
}




