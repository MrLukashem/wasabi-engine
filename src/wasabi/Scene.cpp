
#include "Scene.hpp"

#include "core/Window.hpp"
#include "core/WasabiEngine.hpp"
#include "core/WindowBuilder.hpp"


namespace wasabi {

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::setWindow() noexcept {
	auto window = core::WindowBuilder::create()
		.width(800)
		.height(600)
		.title("Wasabi Window")
		.build();

	m_engine = std::make_unique<core::WasabiEngine>(std::move(window));
}

void Scene::loop() noexcept {
	m_engine->loop();
}

}

