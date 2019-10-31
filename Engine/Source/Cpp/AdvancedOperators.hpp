#pragma once

#include <functional>

//TODO Think of naming convections for Macros
#define noCopy(className) className(const className& other) = delete;\
className& operator=(const className& other) = delete;

#define noMove(className) className(className&& other) = delete;\
className& operator=(className&& other) = delete;

#define noTouch(className) noCopy(className)\
noMove(className)


#define defaultCopy(className) className(const className& other) = default;\
className& operator=(const className& other) = default;

#define defaultMove(className) className(className&& other) = default;\
className& operator=(className&& other) = default;

#define defaultTouch(className) defaultCopy(className)\
defaultMove(className)


#define declCopy(className) className(const className& other);\
className& operator=(const className& other);

#define declMove(className) className(className&& other);\
className& operator=(className&& other);

#define declAllTouch(className) declCopy(className)\
declMove(className)

namespace Hyperion::Cpp {

	//TODO Templatize function/struct for all types
	struct pairFirstElemLess {

		bool operator() (const std::pair<uint32_t, std::function<void()>>& first, const std::pair < uint32_t, std::function<void()>>& second);
	};

}