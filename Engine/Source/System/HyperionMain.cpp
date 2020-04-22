
#include "Application.hpp"

#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"
#include "System/Memory.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Image2D.hpp"

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"



int main()
{
	Hyperion::Application app;
	Hyperion::Rendering::MeshInstance object({}, Hyperion::Rendering::PrimitiveType::Cube);
	Hyperion::Rendering::RenderTarget rt;
	Hyperion::Rendering::RenderTarget rt2;
	rt2 = rt;
	rt.~RenderTarget();


	app.run();
}