
#pragma once

#include "core/Object.hpp"

#include <memory>


namespace wasabi {

using WObject = core::Object;

class Scene {
public:
	Scene& addObject(std::shared_ptr<WObject> object);
	Scene& removeObject(const WObject& object);
	void setWindow();
};

} // namespace wasabi

