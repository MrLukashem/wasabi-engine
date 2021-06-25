
#pragma once

#include "rendering/Renderer.hpp"
#include "components/Transform.hpp"
#include "components/Shape.hpp"

#include <vector>
#include <map>


namespace wasabi::ecs {
class WorldSupervisor;
} // namespace wasabi::ecs

namespace wasabi::rendering {

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
};

using VertexArray = std::vector<Vertex>;

class ShapesRenderingSystem {
public:
	ShapesRenderingSystem(ecs::WorldSupervisor& world);
	void render() noexcept;
private:
	void renderEntity(components::Transform& transform, components::Shape& shape) noexcept;

	ecs::WorldSupervisor& m_world;
	std::map<std::size_t, VertexArray> m_verticesArrays;
};

} // namespace wasabi::rendering
