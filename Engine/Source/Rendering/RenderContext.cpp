#include "RenderContext.hpp"

Hyperion::Rendering::RenderContext::RenderContext(const Configuration& config)
{
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
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(resolution.width, resolution.height, config.applicationName.c_str(), NULL, NULL);

	std::vector<vk::DeviceQueueCreateInfo> deviceQueueInfos = getQueueCreateInfo();
	physDevice = pickGPU();
	vk::PhysicalDeviceFeatures deviceFeatures = getDeviceFeatures();
	device = physDevice.createDevice(
		vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>(deviceQueueInfos.size()),
			deviceQueueInfos.data(),
			0,
			nullptr,
			static_cast<uint32_t>(deviceExtensions.size()),
			deviceExtensions.data(),
			&deviceFeatures
		)
	);

	//vk::

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

vk::PhysicalDevice Hyperion::Rendering::RenderContext::pickGPU()
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

std::array<vk::DeviceQueueCreateInfo, 4> Hyperion::Rendering::RenderContext::getQueueCreateInfo()
{
	//TODO Save additional use of getQueueFamilyIndex
	const std::array<const float, 4> prios = { 0, 0, 0, 0 };
	std::array < vk::DeviceQueueCreateInfo, 4> queueInfos = {
		vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(),
			getQueueFamilyIndex(vk::QueueFlags(VK_QUEUE_GRAPHICS_BIT)),
			1,
			prios.data()
		),
		vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(),
			getQueueFamilyIndex(vk::QueueFlags(VK_QUEUE_COMPUTE_BIT)),
			1,
			prios.data()
		),
		vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(),
			getQueueFamilyIndex(vk::QueueFlags(VK_QUEUE_TRANSFER_BIT)),
			1,
			prios.data()
		)
	};

	return queueInfos;
}

vk::PhysicalDeviceFeatures Hyperion::Rendering::RenderContext::getDeviceFeatures()
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

uint32_t Hyperion::Rendering::RenderContext::getQueueFamilyIndex(vk::QueueFlags flags)
{
	return uint32_t();
}

