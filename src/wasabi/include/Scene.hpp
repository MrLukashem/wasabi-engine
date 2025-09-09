
#pragma once

#include "Object.hpp"
#include "WasabiEngine.hpp"

#include <memory>
#include <vector>


namespace wasabi {

// scene->attach<Transform>(entity, pos);
// scene->attach<Shape>(entity, 

class Scene {
public:
	Scene();
	virtual ~Scene();

	template <typename T>
	T* spawn() {
		// m_objects.emplace_back(m_engine);
		return m_objects.back().get();
	}

	template <typename C, typename... Args>
	void attach(Object& object, Args... args) const noexcept {
        m_engine->attach(object.m_entity, C{args...});
	}

	template <typename C>
	void dettach(Object& object, C&& component) const noexcept {
		m_engine->dettach(object.m_entity, std::forward<C>(component));
	}

	virtual void onUpdate(const float dt) noexcept {};
	virtual void onStart() noexcept {};
	virtual void onClose() noexcept {};
	virtual void setWindow(std::unique_ptr<core::Window> window) noexcept;
	virtual void show() noexcept;
	virtual void loop() noexcept;

private:
	std::unique_ptr<core::WasabiEngine> m_engine;
	std::shared_ptr<core::Window> m_window;
	std::vector<std::unique_ptr<Object>> m_objects;
};

} // namespace wasabi

