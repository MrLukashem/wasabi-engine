
#include "WasabiEngine.hpp"

#include "EventsStack.hpp"
#include "MouseClickEvent.hpp"
#include "OnCloseEvent.hpp"
#include "Window.hpp"
#include "ecs/WorldSupervisor.hpp"
#include "components/Mesh.hpp"
#include "rendering/VulkanRenderer.hpp"

#include "spdlog/spdlog.h"


namespace {

} // namespace

namespace wasabi::core {

using Entity = ecs::Entity;

WasabiEngine::WasabiEngine(std::shared_ptr<Window> window)
	: m_running{false}, m_world{}, m_window{window} {}

WasabiEngine::~WasabiEngine() = default;

void WasabiEngine::loop() noexcept {
	m_running = true;
	EventsStack eventStack(m_window);
	rendering::VulkanRenderer renderer{m_window->getNativeHandle()};

	while (m_running) {
		while (const auto event = eventStack.popEvent()) {
			event->accept(*this);
		}
	}
}

Entity WasabiEngine::createEntity() noexcept {
	return m_world.createEntity();
}

void WasabiEngine::destroyEntity(const Entity entity) noexcept {
	m_world.destroyEntity(entity);
}

void WasabiEngine::visit(MauseClickEvent& event) {
	spdlog::info("MouseClickEvent");
}

void WasabiEngine::visit(OnCloseEvent& event) {
	spdlog::info("OnCloseEvent");
	m_window->close();
	m_running = false;
}

} // namespace wasabi::core
