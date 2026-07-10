//
// Created by ryuu on 27.10.2021.
//

#include "core/GLFWWindow.hpp"
#include <platform/NativeBridge.h>
#include "spdlog/spdlog.h"
#include "MetalLayerHelper.h"

#include <stdexcept>

#include "GLFWKeyEvent.h"
#include "OnCloseEvent.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3native.h>
#endif

namespace wasabi::core {

namespace {

auto logger = spdlog::stdout_color_mt("GLFWWindow");

} // namespace

GLFWWindow::GLFWWindow() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!glfwInit()) {
        throw std::runtime_error("No able to init glfw library!");
    }

    m_glfwWindow = GlfwUPtr(
        glfwCreateWindow(640, 480, "Vulkan Window", nullptr, nullptr),
        [] (auto window) { glfwDestroyWindow(window); });

    if (m_glfwWindow == nullptr) {
        throw std::runtime_error("No able to create window!");
    }

    glfwSetWindowUserPointer(m_glfwWindow.get(), this);
    glfwSetWindowCloseCallback(m_glfwWindow.get(), [](GLFWwindow* window) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self == nullptr) {
            return;
        }
        self->m_events.push(std::make_unique<OnCloseEvent>());
    });
    glfwSetKeyCallback(
    m_glfwWindow.get(),
    [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self == nullptr) {
            return;
        }
        self->m_events.push(std::make_unique<GLFWKeyEvent>(window, key, scancode, action, mods));
    });
}

GLFWWindow::~GLFWWindow() noexcept {
    glfwTerminate();
}

void GLFWWindow::show() noexcept {
    if (m_glfwWindow == nullptr) {
        logger->error("Window is not created!");
    }

    logger->info("Showing window");
    glfwShowWindow(m_glfwWindow.get());
}

void GLFWWindow::close() noexcept {
    if (m_glfwWindow == nullptr) {
        logger->error("Window is not created!");
    }

    logger->info("Closing window");
    glfwWindowShouldClose(m_glfwWindow.get());
}

std::unique_ptr<Event> GLFWWindow::pollEvent() noexcept {
    glfwPollEvents();
    if (m_events.empty()) {
        return nullptr;
    }

    auto event = std::move(m_events.front());
    m_events.pop();
    return event;
}

WindowHandle GLFWWindow::getNativeHandle() const noexcept {
    if (m_glfwWindow == nullptr) {
        return WindowHandle{nullptr};
    }
#if defined(__APPLE__)
    return WindowHandle{createMetalLayer(m_glfwWindow.get())};
#elif defined(__linux__)
    return WindowHandle{glfwGetWaylandDisplay(), glfwGetWaylandWindow(m_glfwWindow.get())};
#endif

    return WindowHandle{nullptr};
}

} // namespace wasabi::core
