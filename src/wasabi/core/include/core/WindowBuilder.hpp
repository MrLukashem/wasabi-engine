
#pragma once

#include <string>
#include <memory>


namespace wasabi::core {

struct Window;

class WindowBuilder {
public:
	WindowBuilder();
	~WindowBuilder();

	std::unique_ptr<Window> build() const noexcept;
	WindowBuilder& width(const uint32_t width) noexcept;
	WindowBuilder& height(const uint32_t height) noexcept;
	WindowBuilder& title(const std::string& title) noexcept;

	static WindowBuilder create() noexcept {
		return WindowBuilder();
	}

private:
	uint32_t m_width;
	uint32_t m_height;
	std::string m_title;
};

} // namespace wasabi::core