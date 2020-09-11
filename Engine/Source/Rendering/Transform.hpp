#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"


namespace Hyperion::Rendering {

	class Transform {//Revisit construction, visual entity as well
	public:
		Transform() = default;
		Transform(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale);

		const glm::vec3& getPosition() const;
		const glm::vec3& getOrientation() const;
		const glm::vec3& getScale() const;

		glm::mat4x4 getModelMatrix() const;
		glm::mat4x4 getInverseModelMatrix() const;

	private:
		glm::vec3 position;
		glm::vec3 orientation;
		glm::vec3 scale;
	};
}