
#pragma once

#include "ecs/Entity.hpp"

#include <algorithm>
#include <bitset>
#include <stack>
#include <iterator>


namespace wasabi::ecs::detail {

using ComponentsTag = std::bitset<128>;

class EntitiesSupervisor {
public:
	EntitiesSupervisor() : idCounter{}, m_freeIds{}, m_entityToTag{} {
		m_freeIds.push(idCounter++);
	}

	EntitiesSupervisor(const EntitiesSupervisor&) = delete;
	EntitiesSupervisor& operator=(const EntitiesSupervisor&) = delete;

	EntitiesSupervisor(EntitiesSupervisor&&) = default;
	EntitiesSupervisor& operator=(EntitiesSupervisor&&) = default;

	Entity createEntity() noexcept {
		return getId();
	}

	void destroyEntity(const Entity entity) {
		m_freeIds.push(entity);
	}

	void setTag(const Entity entity, const ComponentsTag& tag) {
		m_entityToTag[entity] = tag;
	}

	ComponentsTag updateTag(const Entity entity, const ComponentsTag& tag) {
		return m_entityToTag[entity] |= tag;
	}

	ComponentsTag getTag(const Entity entity) const {
		const auto tagItr = m_entityToTag.find(entity);
		return tagItr != m_entityToTag.cend() ? tagItr->second : ComponentsTag{};
	}

	std::vector<Entity> getEntitiesByTag(const ComponentsTag& tag) noexcept {
		std::unordered_map<Entity, ComponentsTag> resultMap{};
		std::vector<Entity> result{};
		std::copy_if(
			m_entityToTag.cbegin(), m_entityToTag.cend(), std::inserter(resultMap, resultMap.end()),
			[&tag](const auto& pair) { return pair.second == tag; });

		result.resize(resultMap.size());
		std::transform(
			resultMap.begin(), resultMap.end(), result.begin(),
			[](const auto& pair) { return pair.first; });

		return result;
	}
private:
	std::size_t getId() noexcept {
		const auto id = m_freeIds.top();
		m_freeIds.pop();
		m_freeIds.push(idCounter++);

		return id;
	}

	std::size_t idCounter;
	std::stack<std::size_t> m_freeIds;
	std::unordered_map<Entity, ComponentsTag> m_entityToTag;
};

} // namespace wasabi::ecs::detail
