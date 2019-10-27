#include "Debug.hpp"

#include <array>
#include <iostream>
#include <unordered_map>

namespace Hyperion::Debug {

	vk::DebugUtilsMessengerEXT debugMessenger;

	//TODO More efficient way of storing mapping, integer log?
	std::unordered_map< VkDebugUtilsMessageSeverityFlagBitsEXT, std::string> debugUtilsMessageSeverityNames = {
		{VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "Verbose"},
		{VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "Info"},
		{VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "Warning"},
		{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "Error"}
	};

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData) {

		(void)messageType;
		(void*)pUserData;

		std::cerr << "VALIDATION LAYER - " << debugUtilsMessageSeverityNames.at(messageSeverity) <<": " << pCallbackData->pMessage << std::endl << std::endl;
		return VK_FALSE;
	}

	//TODO Review C++ bindings style
	void setUpVulkanDebugging(const vk::Instance& instance) {
		
		auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		//Switch to proper logging functions/macros
		if (createFunc == nullptr) {
			throw std::exception("Could not create Messenger");
		}

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity =
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			//VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType =
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;  
		debugCreateInfo.pUserData = nullptr;

		VkDebugUtilsMessengerEXT callback;
		createFunc(instance, &debugCreateInfo, nullptr, &callback);
		debugMessenger = vk::DebugUtilsMessengerEXT(callback);
	}
	void cleanUpVulkanDebugging(const vk::Instance& instance)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, nullptr);
		}
	}
}