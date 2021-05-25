
#pragma once

#include "glm/vec2.hpp"

#include <variant>


namespace wasabi::components {

struct Shape {
	struct Rectangle {
		glm::vec2 size;
	};

	struct Circle {
		float radius;
	};

	Shape() = default;
	Shape(Rectangle rectangle): data{rectangle} {}
	Shape(Circle circle): data{circle} {}

	using Data = std::variant<Rectangle, Circle>;
	Data data;
};

} // namespace wasabi::components

namespace std
{

template<> struct hash<wasabi::components::Shape::Rectangle>
{
	std::size_t operator()(const wasabi::components::Shape::Rectangle& rect) const noexcept
	{
		std::size_t h1 = std::hash<float>{}(rect.size.x);
		std::size_t h2 = std::hash<float>{}(rect.size.y);
		return h1 ^ (h2 << 1);
	}
};

template<> struct hash<wasabi::components::Shape::Circle>
{
	std::size_t operator()(const wasabi::components::Shape::Circle& circle) const noexcept
	{
		return std::hash<float>{}(circle.radius);
	}
};

} // namespace std
