
#include "Transform.hpp"


namespace Hyperion::Rendering {

	Transform::Transform(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale) : position(position), orientation(orientation), scale(scale)
	{
	}

	const glm::vec3& Transform::getPosition() const
	{
		return position;
	}

	const glm::vec3& Transform::getOrientation() const
	{
		return orientation;
	}

	const glm::vec3& Transform::getScale() const
	{
		return scale;
	}

	glm::mat4x4 Transform::getModelMatrix() const {


		return glm::scale(glm::rotate(glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), position), orientation.x, { 1, 0, 0 }), orientation.y, { 0, 1, 0 }), orientation.z, { 0, 0, 1 }), scale);
	}//TODO Highly improvable

	glm::mat4x4 Transform::getInverseModelMatrix() const {
		return glm::inverse(getModelMatrix());
	}
}