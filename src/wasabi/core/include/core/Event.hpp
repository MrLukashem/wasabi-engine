
#pragma once

#include "EventVisitor.hpp"


namespace wasabi::core {

struct Event {
	virtual void accept(EventVisitor& visitor) = 0;
};

} // namespace wasabi::core
