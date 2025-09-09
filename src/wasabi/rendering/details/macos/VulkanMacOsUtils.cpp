//
// Created by Łukasz Merta on 26/11/2021.
//

#ifdef __APPLE__
#include "rendering/details/VulkanUtils.hpp"

namespace wasabi::rendering::details {

std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle) {
    if (nativeHandle.caMetalLayer == nullptr) {
        return {};
    }

    VkMetalSurfaceCreateInfoEXT info{};
    info.pLayer = nativeHandle.caMetalLayer;
    info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    info.pNext = nullptr;
    info.flags = 0;

    VkSurfaceKHR surface{};
    if (vkCreateMetalSurfaceEXT(instance, &info, nullptr, &surface) != VK_SUCCESS) {
        return {};
    }

    return surface;
}

ExtensionsNames getPlatformExtensionsInternal() {
    return {
        VK_EXT_METAL_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
}

} // rendering::details
#endif
