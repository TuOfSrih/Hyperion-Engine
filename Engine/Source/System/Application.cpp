#include "Application.hpp"

#include "Rendering/Image2D.hpp"
#include "Rendering/Mesh.hpp"

#include "GLFW/glfw3.h"

#include <thread>
#include <chrono>

namespace Hyperion {

	const std::string applicationName = "Hyperion Test Program";
	const std::string engineName = "Hyperion Engine";

	Hyperion::Application::Application() : config(Configuration("Hyperion Test", Version(1, 0, 0))), context(Rendering::RenderContext(config))
	{
		Debug::trace("Successfully initialized application!");
	}

	void Hyperion::Application::run()
	{
		Debug::trace("Running application!", 1);
		Rendering::VisualEntity object(Rendering::Transform(glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 }), Rendering::PrimitiveType::Cube, &Rendering::RenderContext::active->getPipelineHandler().getDefaultForward());
		context.reg(&object);//TODO remove again especially cause of destruction
		while (true) {
			glfwPollEvents();
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			object.update();
			context.render();
		}

	}
}


