
#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Object.hpp"


namespace wasabi {

class Scene {
public:
	Scene();
	virtual ~Scene();

	virtual void onAttach(ecs::WorldSupervisor* world) {
		m_world = world;
	}
	virtual void onDetach() {}
	virtual void onUpdate(float dt) noexcept {};

	Object& createObject(const std::string& name) noexcept;
	Object& createTriangle(
		const glm::vec3& p1,
		const glm::vec3& p2,
		const glm::vec3& p3) noexcept;
	Object& createRectangle(
		const glm::vec3& pos,
		const glm::vec2& size) noexcept;
	Object& createCircle(
		const glm::vec3& pos,
		double radius) noexcept;

	void destroyObject(Object& object) noexcept;

private:
	ecs::WorldSupervisor* m_world;
	std::vector<Object> m_objects;
};

} // namespace wasabi::core

