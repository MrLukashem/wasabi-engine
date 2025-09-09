
#pragma once

#include "Event.hpp"

#include <memory>


namespace wasabi::core {

struct Window;

class EventsStack final {
public:
	explicit EventsStack(std::shared_ptr<Window> window);
	~EventsStack() = default;

	std::unique_ptr<Event> popEvent();
private:
	std::shared_ptr<Window> m_window;
};

} // namespace wasabi::core
