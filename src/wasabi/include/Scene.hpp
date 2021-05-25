
#pragma once

#include "Object.hpp"

#include <memory>
#include <vector>


namespace wasabi {

namespace core {
class WasabiEngine;
} // namespace core

// scene->attach<Transform>(entity, pos);
// scene->attach<Shape>(entity, 

class Scene {
public:
	Scene();
	virtual ~Scene();

	template <typename T>
	T* spawn() {
		m_objects.emplace_back(m_engine);
		return m_objects.back().get();
	}

	template <typename C, typename... Args>
	void attach(Object& entity, Args... args) const noexcept {
		m_engine.attach(object.m_entity, C{args...});
	}

	template <typename C>
	void dettach(Object& object, C&& component) const noexcept {
		m_engine.dettach(object.m_entity, std::forward<T>(component));
	}

	virtual void onUpdate(const float dt) noexcept {};
	virtual void onStart() noexcept {};
	virtual void onClose() noexcept {};
	virtual void setWindow() noexcept;
	virtual void loop() noexcept;

private:
	std::unique_ptr<core::WasabiEngine> m_engine;
	std::vector<std::unique_ptr<Object>> m_objects;
};

} // namespace wasabi

