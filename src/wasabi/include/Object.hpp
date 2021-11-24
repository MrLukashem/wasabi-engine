
#pragma once

#include "ecs/Entity.hpp"


namespace wasabi {

class WasabiEngine;

class Object {
public:
	Object(WasabiEngine& engine);
	virtual ~Object() = default;

	WasabiEngine& m_engine;
	const ecs::Entity m_entity;
};

} // namespace wasabi
