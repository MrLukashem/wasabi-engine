
#pragma once

#include "rendering/Renderer.hpp"
#include "components/Transform.hpp"
#include "components/Shape.hpp"
#include "Math.hpp"

#include <vector>
#include <map>


namespace wasabi::ecs {
class WorldSupervisor;
} // namespace wasabi::ecs

namespace wasabi::rendering {

class ShapesRenderingSystem {
public:
	explicit ShapesRenderingSystem(ecs::WorldSupervisor& world);

	void render() noexcept;
private:
	void renderEntity(std::size_t entity, const components::Transform& transform, const components::Shape& shape) noexcept;

	ecs::WorldSupervisor& m_world;
	std::map<std::size_t, Mesh> m_entityHashToMesh;
	std::map<std::size_t, uint32_t> m_entityHashToGpuMesh;
};

} // namespace wasabi::rendering
