#pragma once

#include "Cpp/AdvancedOperators.hpp"

#include "vulkan/vulkan.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <optional>


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
		declMove(VulkanTools);
		noCopy(VulkanTools);
		~VulkanTools();
		
	private:
		vk::DebugUtilsMessengerEXT debugMessenger;
	};

	class StreamCapture {
	public:
		//StreamCapture() = default;
		StreamCapture(std::ios& srcStream, std::ios& dstStream);
		~StreamCapture();

	private:
		std::streambuf* oldStreamBuffer;
		std::ios* oldStream;
	};

	static const std::filesystem::path debugLogName;

	void traceToFile();

	void stopTracingToFile();

	void trace(const std::string& msg, int8_t indentationChange = 0);

	
	__declspec(noreturn) void missingSupport(const char* message);

	__declspec(noreturn) void missingFunctionality(const char* message);

	__declspec(noreturn) void failAssertion(const char* exp, const char* file, const int line);

	__declspec(noreturn) void runtimeError(const char* message);

}

