
#pragma once

#include "ecs/Entity.hpp"
#include "ecs/detail/Container.hpp"
#include "ecs/detail/utils/Constants.hpp"

#include <array>
#include <vector>
#include <queue>


namespace wasabi::ecs::detail {

template <typename C, std::size_t N = utils::MAX_ENTITIES>
class ComponentContainer : public Container {
public:
	struct ComponentData {
		C component;
		bool isContainedByEntity;
	};

	constexpr ComponentContainer() : m_data{} {}

	ComponentContainer(ComponentContainer&& container) = default;
	ComponentContainer& operator=(ComponentContainer&& container) = default;

	ComponentContainer(const ComponentContainer& container) = delete;
	ComponentContainer& operator=(const ComponentContainer& container) = delete;

	void put(const Entity entity, const C& component) {
		m_data[entity] = {std::move(component), true};
	}

	void put(const Entity entity, C&& component) {
		m_data[entity] = {std::move(component), true};
	}

	C& get(const Entity entity) {
		return m_data[entity].component;
	}

	bool isContainedByEntity(const C& component) const {
		return m_data[entity].isContainedByEntity;
	}
private:
	std::array<ComponentData, N> m_data;
};

} // namespace wasabi::ecs::detail
