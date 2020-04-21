
#include "Application.hpp"
#include "Cpp/AdvancedOperators.hpp"
#include "Cpp/Types.hpp"
#include "System/Memory.hpp"
#include "Rendering/Mesh.hpp"

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"



int main()
{

	Hyperion::Application app;
	Hyperion::Rendering::MeshInstance object({}, Hyperion::Rendering::PrimitiveType::Cube);
	
	app.run();
}