#pragma once

#include "Event.hpp"
#include "EventVisitor.hpp"


namespace wasabi::core {

struct EventVisitor;

struct OnCloseEvent : public Event {
	void accept(EventVisitor& visitor) {
		visitor.visit(*this);
	}
};

} // namespace wasabi::core