
#pragma once

#include <memory>
#include <vector>


namespace wasabi::core {

class Window;

class WasabiEngine {
public:
	WasabiEngine(std::shared_ptr<Window> window);
};

} // namespace wasabi::core