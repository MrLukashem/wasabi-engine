
#pragma once

#include "glm/vec3.hpp"

#include <memory>
#include <vector>


namespace wasabi::rendering {
class RenderingComponent;
} // namespace wasabi::rendering

namespace wasabi::core {

class Actor {
public:
	Actor();

	void addComponent(std::unique_ptr<rendering::RenderingComponent> component) noexcept;
private:
	glm::vec3 m_position;
	std::vector<std::unique_ptr<rendering::RenderingComponent>> m_renderingComponents;
};

} // namespace wasabi::core
