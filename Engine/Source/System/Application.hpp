#pragma once

#include "Rendering/RenderContext.hpp"
#include "Configuration.hpp"

namespace Hyperion {

	class Application {

	public:
		Application();

		void run();

	private:
		Rendering::RenderContext context;
		const Configuration config = Configuration("Hyperion Test", Version(1, 0, 0));
	};
}