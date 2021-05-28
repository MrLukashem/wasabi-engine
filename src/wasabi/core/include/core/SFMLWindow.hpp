
#pragma once

#include "Window.hpp"

#include <string>

#include <SFML/Window.hpp>


namespace wasabi::core {

class SFMLWindow : public Window {
public:
	~SFMLWindow();
	SFMLWindow(const uint32_t width, const uint32_t height, const std::string& title);

	void show() const noexcept override;
	void close() noexcept override;
	std::unique_ptr<Event> pollEvent() noexcept override;
	WindowHandle getNativeHandle() const noexcept override;
private:
	sf::Window m_window;
};

} // namespace wasabi::core