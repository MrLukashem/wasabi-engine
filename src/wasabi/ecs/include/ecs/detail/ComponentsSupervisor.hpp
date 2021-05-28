
#pragma once

#include "ecs/Entity.hpp"
#include "ecs/detail/ComponentContainer.hpp"

#include <unordered_map>
#include <type_traits>
#include <memory>


namespace wasabi::ecs::detail {

class ComponentsSupervisor {
public:
	template <typename C>
	C& get(const Entity entity) {
		return getContainerByComponent<std::remove_reference_t<C>>().get(entity);
	}

	template <typename C>
	void bind(const Entity entity, C&& component) {
		getContainerByComponent<std::remove_reference_t<C>>().put(entity, std::forward<C>(component));
	}

	template <typename C>
	bool isContainedByEntity(const Entity entity) const {
		return getContainerByComponent<std::remove_reference_t<C>>().isContainedByEntity(entity);
	}
private:
	template <typename C>
	ComponentContainer<C>& getContainerByComponent() {
		auto& pair = m_typeToContainer.try_emplace(getId<C>(), std::make_unique<ComponentContainer<C>>());
		auto& container = (pair.first)->second;
		return *static_cast<ComponentContainer<C>*>(container.get());
	}

	template <typename C>
	constexpr std::size_t getId() {
		return typeid(C).hash_code();
	}

	std::unordered_map<std::size_t, std::unique_ptr<Container>> m_typeToContainer;
};

} // namespace wasabi::ecs::detail
