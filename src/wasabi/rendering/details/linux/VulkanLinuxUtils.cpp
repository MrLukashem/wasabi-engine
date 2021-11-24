//
// Created by ryuu on 24.10.2021.
//

#ifdef __linux__
#include "rendering/details/linux/VulkanLinuxUtils.hpp"

#include "platform/Config.hpp"

#include "vulkan/vulkan_wayland.h"


namespace wasabi::rendering::details::lx {

std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle) {
    VkWaylandSurfaceCreateInfoKHR info{};
    info.display = nativeHandle.wlDisplay;
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

    VkSurfaceKHR surface{};
    if (vkCreateWaylandSurfaceKHR(instance, &info, nullptr, &surface) != VK_SUCCESS) {
        return {};
    }

    return surface;
}

details::ExtensionsNames getPlatformExtensionsInternal() {
    return {
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };
}

} // rendering::details::linux
#endif
