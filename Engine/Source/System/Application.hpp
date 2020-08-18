#pragma once

#include "Configuration.hpp"

#include "System/Thread.hpp"
#include "Rendering/RenderContext.hpp"


namespace Hyperion {

	class Application {

	public:
		explicit Application();

		void run();

	private:
		System::TaskScheduler taskScheduler;
		Configuration config;
		Rendering::RenderContext context;
		
	};
}