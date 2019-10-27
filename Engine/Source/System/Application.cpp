#include "Application.hpp"

const std::string applicationName = "Hyperion Test Program";
const std::string engineName = "Hyperion Engine";

Hyperion::Application::Application(): context(Rendering::RenderContext(config))
{

}

void Hyperion::Application::run()
{
	//while (true);
}
