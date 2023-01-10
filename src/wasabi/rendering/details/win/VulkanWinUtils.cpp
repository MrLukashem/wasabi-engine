//
// Created by ryuu on 24.10.2021.
//

#ifdef _WIN32
#include "rendering/details/win/VulkanWinUtils.hpp"

#include <optional>


namespace wasabi::rendering::details::win {

std::optional<VkSurfaceKHR> createVkSurfaceInternal(VkInstance instance, WindowHandle nativeHandle) {
    VkWin32SurfaceCreateInfoKHR info{};
    info.hinstance = GetModuleHandle(nullptr);
    info.hwnd = nativeHandle;
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

    VkSurfaceKHR surface{};
    if (vkCreateWin32SurfaceKHR(instance, &info, nullptr, &surface) != VK_SUCCESS) {
        logger->error("failed to create window surface!");
        return {};
    }

    return surface;
}

details::ExtensionsNames getPlatformExtensionsInternal() {
	if (wasabi::getPlatform() == wasabi::Platform::WINDOWS) {
		return {
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		};
	}

	return {};
}

} // wasabi::rendering::details::win
#endif
