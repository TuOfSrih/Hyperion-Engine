#pragma once

#include "Transform.hpp"


namespace Hyperion::Rendering {

	class Camera {
	public:
		Camera(const Transform& transform) : transform(transform), FoVAngle(FoVAngle) {};

		glm::mat4x4 getViewMatrix();
		glm::mat4x4 getProjectionMatrix();
		
		static Camera& getActive();
	private:

		Transform transform;
		float FoVAngle = 60.0f;
		float near = 0.01f;
		float far = 50.0f;

	};
}