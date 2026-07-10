
#include "WasabiEngine.hpp"

#include <thread>

#include "EventsStack.hpp"
#include "MouseClickEvent.hpp"
#include "OnCloseEvent.hpp"
#include "Window.hpp"
#include "ecs/WorldSupervisor.hpp"
#include "components/Mesh.hpp"
#include "rendering/VulkanRenderer.hpp"

#include "../../external/spdlog/include/spdlog/spdlog.h"


namespace {

} // namespace

namespace wasabi {

using namespace core;
using Entity = ecs::Entity;

WasabiEngine::WasabiEngine(std::shared_ptr<Window> window)
	: m_running{false}, m_world{}, m_window{window} {}

WasabiEngine::~WasabiEngine() = default;

void WasabiEngine::setScene(std::unique_ptr<Scene> scene) noexcept {
	m_scene = std::move(scene);
	m_scene->onAttach(&m_world);
}

void WasabiEngine::loop() noexcept {
	m_running = true;
	EventsStack eventStack(m_window);
	rendering::VulkanRenderer renderer{
		m_window->getNativeHandle(),
		{
			rendering::ShaderInfo{
				.stage = rendering::ShaderStage::Vertex,
				.path = std::string(WASABI_SHADER_DIR) + "/vert.spv"
			},
			rendering::ShaderInfo{
				.stage = rendering::ShaderStage::Fragment,
				.path = std::string(WASABI_SHADER_DIR) + "/frag.spv"
			}
		}
	};

	m_window->show();

	while (m_running) {
		while (const auto event = eventStack.popEvent()) {
			if (event != nullptr) {
				event->accept(*this);
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

		renderer.drawFrame();

		m_scene->onUpdate(0.0f);
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
