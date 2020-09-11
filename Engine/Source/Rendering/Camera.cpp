#include "Camera.hpp"


#include "RenderContext.hpp"

namespace Hyperion::Rendering {
	
	glm::mat4x4 Camera::getViewMatrix()
	{//TODO properly implement view matrix, why not y up
		return glm::lookAt(transform.getPosition(), { 0, 0, 0 }, { 0, 1, 0 });
		//return transform.getInverseModelMatrix();
	}
	glm::mat4x4 Camera::getProjectionMatrix()
	{
		const VideoSettings settings = RenderContext::active->getVideoSettings();

		auto result = glm::perspective(glm::radians(FoVAngle), static_cast<float>(settings.resolution.width) / settings.resolution.height, near, far);
		result[1][1] *= -1;

		return result;
	}

	Camera& Hyperion::Rendering::Camera::getActive()
	{
		static Camera cam(Transform({ 10, 10, 10 }, {}, { 1, 1, 1 }));
		return cam;
	}
}


