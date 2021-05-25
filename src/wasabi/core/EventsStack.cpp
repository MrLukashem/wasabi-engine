
#include "EventsStack.hpp"

#include "Window.hpp"


namespace wasabi::core {

EventsStack::EventsStack(std::shared_ptr<Window> window)
	: m_window{window} {
}

std::unique_ptr<Event> EventsStack::popEvent() {
	return m_window->pollEvent();
}

} // namespace wasabi::core
