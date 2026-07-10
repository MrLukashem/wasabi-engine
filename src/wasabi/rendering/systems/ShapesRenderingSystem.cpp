
#include "rendering/systems/ShapesRenderingSystem.hpp"

#include "rendering/VulkanRenderer.hpp"
#include "ecs/WorldSupervisor.hpp"

// #include <vulkan/vulkan.hpp>
#define VK_USE_PLATFORM_MACOS_MVK
#include <volk.h>


namespace {

using wasabi::Vertex;
using wasabi::Mesh;
using namespace wasabi::rendering;
using namespace wasabi::components;

std::size_t makeHash(const Shape& shape) noexcept {
	return std::hash<Shape::Data>{}(shape.data);
}

Mesh createMesh(const Shape::Rectangle& rect) noexcept {
	const float halfX = rect.size.x / 2.0f;
	const float halfY = rect.size.y / 2.0f;
	return {
		// Vertex{{-halfX, -halfY, .0f}, {.0f, .0f, .0f}},
		// Vertex{{-halfX, halfY, .0f}, {.0f, .0f, .0f}},
		// Vertex{{halfX, halfY, .0f}, {.0f, .0f, .0f}},
		// Vertex{{halfX, -halfY, .0f}, {.0f, .0f, .0f}},
	};
}

Mesh createMesh(const Shape::Triangle& triangle) noexcept {
	const auto t = triangle;
	return {
		// Vertex{{t.pos1.x, t.pos1.y, t.pos1.z}, {.0f, .0f, .0f}},
		// Vertex{{t.pos2.x, t.pos2.y, t.pos2.z}, {.0f, .0f, .0f}},
		// Vertex{{t.pos3.x, t.pos3.y, t.pos3.z}, {.0f, .0f, .0f}},
	};
}

Mesh createMesh(const Shape::Circle& circle) noexcept {
	return {};
}

Mesh createMesh(const Shape& shape) noexcept {
	return std::visit([] (auto&& data) { return createMesh(data); }, shape.data);
}

} // namespace

namespace wasabi::rendering {

using namespace wasabi::components;

ShapesRenderingSystem::ShapesRenderingSystem(ecs::WorldSupervisor& world)
	: m_world{world} {}

void ShapesRenderingSystem::render() noexcept {
	m_world.each<Transform, Shape>([this](const auto entity) {
		renderEntity(
			entity,
			m_world.get<Transform>(entity),
			m_world.get<Shape>(entity));
	});
}

void ShapesRenderingSystem::renderEntity(
	const ecs::Entity entity,
	const Transform& transform,
	const Shape& shape)
noexcept {
	if (m_entityHashToGpuMesh.contains(entity)) {
		return;
	}

	const auto hash = makeHash(shape);
	m_entityHashToMesh[hash] = createMesh(shape);
}

} // namespace wasabi::rendering
