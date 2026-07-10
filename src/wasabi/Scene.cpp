#include "Scene.hpp"

#include "components/Shape.hpp"


namespace wasabi {

using components::Shape;

Scene::Scene() = default;

Scene::~Scene() = default;

Object& Scene::createObject(const std::string& name) noexcept {
    m_objects.emplace_back(m_world->createEntity(), m_world);
    return m_objects.back();
}

Object& Scene::createTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) noexcept {
    auto &obj = createObject("Triangle");
    obj.addComponent<Shape>(Shape::Triangle{p1, p2, p3});

    return obj;
}

Object& Scene::createRectangle(const glm::vec3& pos, const glm::vec2& size) noexcept {
    auto &obj = createObject("Rectangle");
    return obj;
}

Object& Scene::createCircle(const glm::vec3& pos, double radius) noexcept {
    auto &obj = createObject("Circle");
    return obj;
}

void Scene::destroyObject(Object& object) noexcept {
    std::erase_if(m_objects, [&object](const Object &obj) { return obj == object; });
}

}
