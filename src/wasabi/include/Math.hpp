//
// Created by Łukasz Merta on 12/09/2025.
//

#pragma once

#include <glm/glm.hpp>

#include <vector>


namespace wasabi {

struct Vec2 {
    float x, y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(const float x, const float y) : x(x), y(y) {}
    Vec2(const glm::vec2& v) : x(v.x), y(v.y) {}

    operator glm::vec2() const { return glm::vec2{x, y}; }
};

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
    Vec3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}

    operator glm::vec3() const { return glm::vec3{x, y, z}; }
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

} // namespace wasabi
