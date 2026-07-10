
#pragma once

#include "glm/vec2.hpp"

#include <variant>


namespace wasabi::components {

struct Shape {
	struct Triangle {
		glm::vec3 pos1, pos2, pos3;
	};

	struct Rectangle {
		glm::vec2 size;
	};

	struct Circle {
		float radius;
	};

	Shape() = default;
	Shape(Triangle triangle): data{triangle} {}
	Shape(Rectangle rectangle): data{rectangle} {}
	Shape(Circle circle): data{circle} {}

	using Data = std::variant<Triangle, Rectangle, Circle>;
	Data data;
};

} // namespace wasabi::components

namespace std
{

template<> struct hash<wasabi::components::Shape::Triangle>
{
	std::size_t operator()(const wasabi::components::Shape::Triangle& triangle) const noexcept
	{
		std::size_t result_hash = 0;

		auto combine_hashes = [&] (auto value) {
			result_hash ^= std::hash<decltype(value)>{}(value) + 0x9e3779b9 + (result_hash << 6) + (result_hash >> 2);
		};

		combine_hashes(triangle.pos1.x);
		combine_hashes(triangle.pos1.y);
		combine_hashes(triangle.pos1.z);

		combine_hashes(triangle.pos2.x);
		combine_hashes(triangle.pos2.y);
		combine_hashes(triangle.pos2.z);

		combine_hashes(triangle.pos3.x);
		combine_hashes(triangle.pos3.y);
		combine_hashes(triangle.pos3.z);

		return result_hash;
	}
};

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
