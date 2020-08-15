#include "Application.hpp"

#include <thread>
#include <chrono>

const std::string applicationName = "Hyperion Test Program";
const std::string engineName = "Hyperion Engine";

Hyperion::Application::Application(): context(Rendering::RenderContext(config))
{

}

void Hyperion::Application::run()
{
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
		context.render();
	}

}
