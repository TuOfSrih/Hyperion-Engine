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
		Rendering::RenderContext context;
		const Configuration config = Configuration("Hyperion Test", Version(1, 0, 0));
	};
}