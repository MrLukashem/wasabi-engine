
#pragma once

#include <glm/vec3.hpp>


namespace wasabi::components {

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

} // namespace wasabi::components
