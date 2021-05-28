
#include "ecs/WorldSupervisor.hpp"


namespace wasabi::ecs {

Entity WorldSupervisor::createEntity() noexcept {
	return m_entitiesSupervisor.createEntity();
}

void WorldSupervisor::destroyEntity(const Entity entity) noexcept {
	m_entitiesSupervisor.destroyEntity(entity);
}

} // namespace wasabi::ecs
