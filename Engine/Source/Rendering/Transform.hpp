#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"


namespace Hyperion::Rendering {

	class Transform {//Revisit construction, visual entity as well
	public:
		Transform() = default;
		Transform(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale);

		glm::mat4x4 getModelMatrix() const;
		glm::mat4x4 getInverseModelMatrix() const;

	private:
		glm::vec3 position;
		glm::vec3 orientation;
		glm::vec3 scale;
	};
}