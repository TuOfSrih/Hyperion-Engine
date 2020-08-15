#include "Camera.hpp"


#include "RenderContext.hpp"

namespace Hyperion::Rendering {
	
	glm::mat4x4 Camera::getViewMatrix()
	{
		return transform.getInverseModelMatrix();
	}
	glm::mat4x4 Camera::getProjectionMatrix()
	{
		const VideoSettings settings = RenderContext::active->getVideoSettings();
		return glm::perspective(glm::radians(FoVAngle), static_cast<float>(settings.resolution.width) / settings.resolution.height, near, far);
	}
	Camera& Hyperion::Rendering::Camera::getActive()
	{
		static Camera cam({});
		return cam;
	}
}


