
#pragma once

#include "Window.hpp"
#include "Scene.hpp"
#include "EventVisitor.hpp"
#include "ecs/WorldSupervisor.hpp"

#include <memory>


namespace wasabi {

//struct Window;

class WasabiEngine : public core::EventVisitor {
public:
	explicit WasabiEngine(std::shared_ptr<core::Window> window);
	~WasabiEngine();

	WasabiEngine(const WasabiEngine&) = delete;
	WasabiEngine(WasabiEngine&&) = default;

	WasabiEngine& operator=(const WasabiEngine&);
	WasabiEngine& operator=(WasabiEngine&&) = default;

	void setScene(std::unique_ptr<Scene> scene) noexcept;

	ecs::Entity createEntity() noexcept;

	void destroyEntity(const ecs::Entity entity) noexcept;
	void loop() noexcept;

	template <typename C>
	void attach(const ecs::Entity entity, C&& component) noexcept {
		m_world.attach(entity, std::forward<C>(component));
	}

	template <typename C>
	void dettach(const ecs::Entity entity, C&& component) noexcept {
		m_world.dettach(entity, std::forward<C>(component));
	}

	void visit(core::MauseClickEvent& event) override;
	void visit(core::OnCloseEvent& event) override;
private:
	bool m_running;
	ecs::WorldSupervisor m_world;
	std::shared_ptr<core::Window> m_window;
	std::unique_ptr<Scene> m_scene;
};

} // namespace wasabi