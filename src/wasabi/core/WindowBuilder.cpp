
#include "WindowBuilder.hpp"

#include "SFMLWindow.hpp"
#include "GLFWWindow.hpp"


namespace wasabi::core {

WindowBuilder::WindowBuilder() : m_height{}, m_width{}, m_title{} {}

WindowBuilder::~WindowBuilder() = default;

std::unique_ptr<Window> WindowBuilder::build() const noexcept {
#ifdef _WIN32
    return std::make_unique<SFMLWindow>(m_width, m_height, m_title);
#endif
#ifdef __linux__
    return std::make_unique<GLFWWindow>();
#endif
}

WindowBuilder& WindowBuilder::width(const uint32_t width) noexcept {
	m_width = width;
	return *this;
}

WindowBuilder& WindowBuilder::height(const uint32_t height) noexcept {
	m_height = height;
	return *this;
}

WindowBuilder& WindowBuilder::title(const std::string& title) noexcept {
	m_title = title;
	return *this;
}

} // namespace wasabi::core
