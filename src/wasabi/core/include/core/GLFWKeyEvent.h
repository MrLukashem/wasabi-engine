//
// Created by Łukasz Merta on 06/09/2025.
//

#pragma once

#include <memory>

#include "Event.hpp"
#include <GLFW/glfw3.h>

namespace wasabi::core {

class GLFWKeyEvent: public Event {
public:
    GLFWKeyEvent(
        GLFWwindow* window,
        const int key, const int scancode, const int action, const int mods)
    : m_window{window}, m_key{key}, m_scancode{scancode}, m_action{action}, m_mods{mods} {}

    virtual ~GLFWKeyEvent() = default;

    void accept(EventVisitor& visitor) override {};

private:
    GLFWwindow* m_window;
    int m_key;
    int m_scancode;
    int m_action;
    int m_mods;
};

} // namespace wasabi::core
