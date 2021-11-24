//
// Created by ryuu on 27.10.2021.
//

#include "core/GLFWWindow.hpp"

#include <stdexcept>


namespace wasabi::core {

GLFWWindow::GLFWWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("No able to init glfw library!");
    }
}

GLFWWindow::~GLFWWindow() noexcept {
    glfwTerminate();
}

void GLFWWindow::show() noexcept {
    m_glfwWindow = GlfwUPtr(
        glfwCreateWindow(640, 480, "Vulkan Window", nullptr, nullptr),
        [] (auto window) { glfwDestroyWindow(window); });
}

void GLFWWindow::close() noexcept {
    glfwWindowShouldClose(m_glfwWindow.get());
}

std::unique_ptr<Event> GLFWWindow::pollEvent() noexcept {

}

WindowHandle GLFWWindow::getNativeHandle() const noexcept {
}

} // namespace wasabi::core
