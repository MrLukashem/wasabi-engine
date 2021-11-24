
#pragma once

#include "platform/WindowHandle.hpp"

#include <memory>


namespace wasabi::core {

struct Event;

struct Window {
	virtual ~Window() = default;

	virtual void show() noexcept = 0;
	virtual void close() noexcept = 0;
	virtual std::unique_ptr<Event> pollEvent() noexcept = 0;
	virtual WindowHandle getNativeHandle() const noexcept = 0;
};

} // namespace wasabi::core