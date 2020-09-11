
#include "Application.hpp"
#include "Debug.hpp"

using namespace Hyperion::Rendering;

int main()
{
	Hyperion::Debug::traceToFile();
	Hyperion::Application app;


	app.run();

	Hyperion::Debug::stopTracingToFile();
}