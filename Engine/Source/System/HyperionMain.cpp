
#include "Application.hpp"

#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"
#include "System/Memory.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Image2D.hpp"

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"

using namespace Hyperion::Rendering;

int main()
{
	Hyperion::Application app;
	VisualEntity object(Transform(glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 0, 0 }, glm::vec3{1, 1, 1}), nullptr, &RenderContext::active->getPipelineHandler().getDefaultForward());
	//Hyperion::Rendering::RenderTarget rt;


	app.run();
}