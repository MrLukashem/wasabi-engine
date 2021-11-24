
#pragma once

#include "ecs/detail/ComponentsSupervisor.hpp"
#include "ecs/detail/EntitiesSupervisor.hpp"

#include <functional>


namespace wasabi::ecs {

class WorldSupervisor {
	using ComponentHash = std::size_t;
	using Position = std::size_t; // ComponentPosInTag

public:
	Entity createEntity() noexcept;
	void destroyEntity(const Entity entity) noexcept;

	template <typename C>
	void attach(const Entity entity, C&& component) noexcept {
		m_componentsSupervisor.bind(std::forward<C>(component), entity);

		m_hashToId[getHash<C>()] = ++m_componentIdCounter;
		const auto& tag = m_entitiesSupervisor.updateTag(
			entity, detail::ComponentsTag().set(m_componentIdCounter, true));

		const auto& viewItr = m_worldViews.find(tag);
		if (viewItr == m_worldViews.end()) {
			return;
		}

		viewItr->second.push_back(entity);
	}

	template <typename C>
	void dettach(const Entity entity, C&& component) noexcept {
		m_entitiesSupervisor.updateTag(
			m_entitiesSupervisor.getTag(entity).set(m_hashToId[getHash<C>()], false));
	}

	template <typename C>
	C& get(const Entity entity) noexcept {
		return m_componentsSupervisor.get<C>(entity);
	}

	template <typename... Cs>
	std::vector<Entity> getEntities() noexcept {
		const auto tag = createTag<Cs...>(detail::ComponentsTag());
		const auto& viewItr = m_worldViews.find(tag);
		if (viewItr == m_worldViews.end()) {
			return m_entitiesSupervisor.getEntitiesByTag(tag);
		}

		return viewItr->second;
	}

	template <typename... Cs>
	void each(const std::function<void(Entity)>& callback) noexcept {
		for (const auto entity : getEntities<Cs...>()) {
			callback(entity);
		}
	}
private:
	template <typename C>
	detail::ComponentsTag createTag(detail::ComponentsTag tag) const noexcept {
		const auto idItr = m_hashToId.find(getHash<C>());
		return idItr != m_hashToId.cend() ? tag.set(idItr->second, true) : tag;
	}

	template <typename C1, typename C2, typename... Cs>
	detail::ComponentsTag createTag(detail::ComponentsTag tag) const noexcept {
		const auto idItr = m_hashToId.find(getHash<C1>());
		return idItr != m_hashToId.cend()
			? createTag<C2, Cs...>(tag.set(idItr->second, true)) : createTag<C2, Cs...>(tag);
	}

	template <typename T>
	std::size_t getHash() const noexcept {
		return typeid(T).hash_code();
	}

	// registerWorldView
	// unregisterWorldView
	detail::EntitiesSupervisor m_entitiesSupervisor;
	detail::ComponentsSupervisor m_componentsSupervisor;

	// tag -> vector<entities>
	std::size_t m_componentIdCounter;
	std::unordered_map<detail::ComponentsTag, std::vector<Entity>> m_worldViews;
	std::unordered_map<ComponentHash, Position> m_hashToId;
};

} // namespace wasabi::ecs
