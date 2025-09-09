
#include "Scene.hpp"

#include "core/Window.hpp"
#include "core/WasabiEngine.hpp"
#include "core/WindowBuilder.hpp"


namespace wasabi {

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::setWindow(std::unique_ptr<core::Window> window) noexcept {
	if (window == nullptr) {
		return;
	}

	m_window = std::move(window);
	m_engine = std::make_unique<core::WasabiEngine>(m_window);
}

void Scene::show() noexcept {
	if (m_window == nullptr) {
		return;
	}

	m_window->show();
}

void Scene::loop() noexcept {
	m_engine->loop();
}

}

