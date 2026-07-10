
#pragma once

#include "ecs/Entity.hpp"
#include "ecs/WorldSupervisor.hpp"


namespace wasabi {

class Object final {
public:
	explicit Object(const ecs::Entity entity, ecs::WorldSupervisor* world)
		: m_entity{entity}, m_world{world} {}

	virtual ~Object() {
		m_world->destroyEntity(m_entity);
	}

	bool operator==(const Object& obj) const {
		return m_entity == obj.m_entity;
	}

	template <typename C, typename... Args>
	void addComponent(Args&&... args) {
		m_world->attach<C>(m_entity, std::forward<Args...>(args...));
	}

	template <typename C>
	void removeComponent() {
		m_world->dettach<C>(m_entity);
	}

	template <typename C>
	C& getComponent() {
		return m_world->get<C>(m_entity);
	}
private:
	ecs::Entity m_entity;
	ecs::WorldSupervisor* m_world;
};

} // namespace wasabi
