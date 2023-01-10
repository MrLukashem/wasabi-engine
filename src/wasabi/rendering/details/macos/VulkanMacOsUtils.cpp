//
// Created by Łukasz Merta on 26/11/2021.
//

#ifdef __APPLE__
#include "rendering/details/VulkanUtils.hpp"


namespace wasabi::rendering::details {

std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle) {
    VkMetalSurfaceCreateInfoEXT info{};
    info.pLayer = nativeHandle.caMetalLayer;
    info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;

    VkSurfaceKHR surface{};
    if (vkCreateMetalSurfaceEXT(instance, &info, nullptr, &surface) != VK_SUCCESS) {
        return {};
    }

    return surface;
}

details::ExtensionsNames getPlatformExtensionsInternal() {
    return {
        VK_EXT_METAL_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };
}

} // rendering::details
#endif
