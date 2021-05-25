
#pragma once

#include <memory>


namespace wasabi::core {

struct Event;

struct Window {
	virtual ~Window() = default;

	virtual void show() const noexcept = 0;
	virtual void close() noexcept = 0;
	virtual std::unique_ptr<Event> pollEvent() noexcept = 0;
};

} // namespace wasabi::core