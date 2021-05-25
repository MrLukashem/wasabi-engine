
#include "rendering/systems/ShapesRenderingSystem.hpp"

#include "rendering/VulkanRenderer.hpp"
#include "ecs/WorldSupervisor.hpp"

#include <vulkan/vulkan.hpp>


namespace {
} // namespace

namespace wasabi::rendering {

using namespace wasabi::components;

ShapesRenderingSystem::ShapesRenderingSystem(ecs::WorldSupervisor& world)
	: m_world{world} {}

void ShapesRenderingSystem::render() noexcept {
	m_world.each<Transform, Shape>([this](const auto entity) {
		renderEntity(
			m_world.get<Transform>(entity),
			m_world.get<Shape>(entity));
	});
}

void ShapesRenderingSystem::renderEntity(Transform& transform, Shape& shape) noexcept {
	const auto hash = makeHash(shape);
	const auto& arrayItr = m_verticesArrays.find(hash);
	if (arrayItr == m_verticesArrays.end()) {
		m_verticesArrays[hash] = createVertexArray(shape);
	}

	m_verticesArrays[hash];
}

std::size_t ShapesRenderingSystem::makeHash(const Shape& shape) const noexcept {
	return std::hash<Shape::Data>{}(shape.data);
}

VertexArray ShapesRenderingSystem::createVertexArray(const Shape& shape) const noexcept {
	return std::visit([this](auto&& shape) { return createVertexArray(shape); }, shape.data);
}

VertexArray ShapesRenderingSystem::createVertexArray(const Shape::Rectangle& rect) const noexcept {
	const float halfX = rect.size.x / 2.0f;
	const float halfY = rect.size.y / 2.0f;
	return {
		Vertex{{-halfX, -halfY, .0f}, {.0f, .0f, .0f}},
		Vertex{{-halfX, halfY, .0f}, {.0f, .0f, .0f}},
		Vertex{{halfX, halfY, .0f}, {.0f, .0f, .0f}},
		Vertex{{halfX, -halfY, .0f}, {.0f, .0f, .0f}},
	};
}

VertexArray ShapesRenderingSystem::createVertexArray(const Shape::Circle& circle) const noexcept {
	return {};
}

} // namespace wasabi::rendering
