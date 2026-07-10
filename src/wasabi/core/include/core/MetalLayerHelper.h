//
// Created by Łukasz Merta on 04/09/2025.
//

#pragma once

#include <GLFW/glfw3.h>

#ifdef __APPLE__

#ifdef __cplusplus
extern "C" {
#endif

    CAMetalLayer* createMetalLayer(GLFWwindow* glfwWindow);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__