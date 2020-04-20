#include "Debug.hpp"

#include <array>
#include <iostream>
#include <unordered_map>
#include <sstream>

namespace Hyperion::Debug {


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

		std::cout << "VALIDATION LAYER - " << debugUtilsMessageSeverityNames.at(messageSeverity) <<": " << pCallbackData->pMessage << std::endl << std::endl;
		return VK_FALSE;
	}


	//TODO Review C++ bindings style + Check messenger setup
	VulkanTools::VulkanTools(const vk::Instance& instance): instance(instance)
	{
		auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		//Switch to proper logging functions/macros
		if (createFunc == nullptr) {
			throw std::exception("Failed to acquire function pointers of debug messenger creation");
		}
		
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity =
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType =
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;
		debugCreateInfo.pUserData = nullptr;

		VkDebugUtilsMessengerEXT messenger;
		if(createFunc(instance, &debugCreateInfo, nullptr, &messenger)) Debug::missingFunctionality("Failed to create Vulkan debug messenger");
		debugMessenger = vk::DebugUtilsMessengerEXT(messenger);
	}
	VulkanTools::~VulkanTools()
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, nullptr);
		}
	}
	void missingSupport(const char* message)
	{
		throw std::exception(message);
	}
	void missingFunctionality(const char* message)
	{
		std::cerr << message << std::endl;;
	}
	void failAssertion(const char* exp, const char* file, const int line)
	{

		std::stringstream ss;
		ss << "Assertion " << exp << "failed in " << file << "at line " << line;
		throw std::exception(ss.str().c_str());
	}
	void runtimeError(const char* message)
	{
		throw std::exception(message);
	}
}