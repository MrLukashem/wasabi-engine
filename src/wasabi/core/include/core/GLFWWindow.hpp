//
// Created by ryuu on 27.10.2021.
//

#pragma once

#include "core/Window.hpp"

#include <GLFW/glfw3.h>

#include <functional>


namespace wasabi::core {

class GLFWWindow : public Window {
public:
    GLFWWindow();
    ~GLFWWindow() noexcept;

    void show() noexcept override;
    void close() noexcept override;
    std::unique_ptr<Event> pollEvent() noexcept override;
    WindowHandle getNativeHandle() const noexcept override;
private:
    using GlfwUPtr = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>;
    GlfwUPtr m_glfwWindow;
};

} // namespace wasabi::core
