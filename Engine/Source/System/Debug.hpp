#pragma once

#include "vulkan/vulkan.hpp"
#include "Cpp/AdvancedOperators.hpp"


#ifdef _DEBUG
#define ASSERT(EXPR) ((EXPR? (void)0 : Debug::failAssertion(#EXPR, __FILE__, __LINE__)))
#else
#define ASSERT(EXPR) ((void)0)
#endif



namespace Hyperion::Debug {

	class VulkanTools {
	public:
		VulkanTools() = default;
		VulkanTools(const vk::Instance& instance);
		defaultMove(VulkanTools);
		noCopy(VulkanTools);
		~VulkanTools();
		
	private:
		vk::Instance instance;
		vk::DebugUtilsMessengerEXT debugMessenger;
	};

	void missingSupport(const char* message);

	void missingFunctionality(const char* message);

	void failAssertion(const char* exp, const char* file, const int line);

}

