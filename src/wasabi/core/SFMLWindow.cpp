
#pragma once

#include "SFMLWindow.hpp"

#include "MouseClickEvent.hpp"
#include "OnCloseEvent.hpp"

#include <map>


namespace {
namespace details {

wasabi::core::MauseClickEvent::ButtonType toButtonType(sf::Mouse::Button button) noexcept {
	using ButtonType = wasabi::core::MauseClickEvent::ButtonType;

	static std::map<sf::Mouse::Button, ButtonType> toButtonTypeMap = {
		{sf::Mouse::Button::Right, ButtonType::RIGHT},
		{sf::Mouse::Button::Left, ButtonType::LEFT}
	};

	return ButtonType::UNKNOWN;
}

} // namespace details
} // namespace

namespace wasabi::core {

SFMLWindow::SFMLWindow(const uint32_t width, const uint32_t height, const std::string& title)
	: m_window{sf::VideoMode(width, height), title} {
}

SFMLWindow::~SFMLWindow() = default;

void SFMLWindow::show() const noexcept {
}

void SFMLWindow::close() noexcept {
	m_window.close();
}

std::unique_ptr<Event> SFMLWindow::pollEvent() noexcept {
	sf::Event event{};
	if (!m_window.pollEvent(event)) {
		return nullptr;
	}

	// TODO: event factory here required
	switch (event.type) {
		case sf::Event::MouseButtonPressed:
			return std::make_unique<MauseClickEvent>(
				event.mouseButton.x,
				event.mouseButton.y,
				details::toButtonType(event.mouseButton.button));
		case sf::Event::Closed:
			return std::make_unique<OnCloseEvent>();
		default:
			return nullptr;
	}
}

} // namespace wasabi::core
