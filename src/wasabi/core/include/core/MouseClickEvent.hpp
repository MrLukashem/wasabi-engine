
#pragma once

#include "Event.hpp"
#include "EventVisitor.hpp"

namespace wasabi::core {

struct EventVisitor;

struct MauseClickEvent : public Event {
	enum class ButtonType {
		LEFT,
		RIGHT,
		UNKNOWN,
	};

	MauseClickEvent(int x, int y, ButtonType buttonType)
		: x{ x }, y{ y }, buttonType{ buttonType } {}

	void accept(EventVisitor& visitor) {
		visitor.visit(*this);
	}

	int x;
	int y;
	ButtonType buttonType;
};

} // namespace wasabi::core
