
#pragma once

#include "Event.hpp"

#include <memory>


namespace wasabi::core {

struct Window;

class EventsStack {
public:
	EventsStack(std::shared_ptr<Window> window);
	virtual ~EventsStack() = default;

	std::unique_ptr<Event> popEvent();
private:
	std::shared_ptr<Window> m_window;
};

} // namespace wasabi::core
