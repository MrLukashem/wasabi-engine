
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

extern CAMetalLayer* getMetalLayer(NSWindow* window);

extern "C" CAMetalLayer* createMetalLayer(GLFWwindow* glfwWindow) {
    NSWindow* nsWindow = (NSWindow*)glfwGetCocoaWindow(glfwWindow);
    return getMetalLayer(nsWindow);
}
