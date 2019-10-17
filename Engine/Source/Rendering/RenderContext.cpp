#include "RenderContext.hpp"

Hyperion::Rendering::RenderContext::RenderContext(const Configuration& config)
{
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

}

