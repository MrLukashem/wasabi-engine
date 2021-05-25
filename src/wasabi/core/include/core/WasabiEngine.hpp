
#pragma once

#include "Window.hpp"
#include "EventVisitor.hpp"
#include "ecs/WorldSupervisor.hpp"

#include <memory>


namespace wasabi::core {

//struct Window;

class WasabiEngine : public EventVisitor {
public:
	WasabiEngine(std::shared_ptr<Window> window);
	~WasabiEngine();

	WasabiEngine(const WasabiEngine&) = delete;
	WasabiEngine(WasabiEngine&&) = default;

	WasabiEngine& operator=(const WasabiEngine&) = delete;
	WasabiEngine& operator=(WasabiEngine&&) = default;

	ecs::Entity createEntity() noexcept;

	void destroyEntity(const ecs::Entity entity) noexcept;
	void loop() noexcept;

	template <typename C>
	void attach(const ecs::Entity entity, C&& component) noexcept {
		m_world.addComponent(entity, std::forward<C>(component));
	}

	template <typename C>
	void dettach(const ecs::Entity entity, C&& component) noexcept {
		m_world.removeComponent(entity, std::forward<T>(component));
	}

	void visit(MauseClickEvent& event) override;
	void visit(OnCloseEvent& event) override;
private:
	bool m_running;
	ecs::WorldSupervisor m_world;
	std::shared_ptr<Window> m_window;
};

} // namespace wasabi::core