//
// Created by Łukasz Merta on 27/11/2021.
//

#pragma once

#include "platform/WindowHandle.hpp"

#ifdef __APPLE__
#define VK_USE_PLATFORM_MACOS_MVK
#endif
#include <volk.h>

#include <optional>
#include <vector>


namespace wasabi::rendering::details {
    using ExtensionsNames = std::vector<const char *>;
}

namespace wasabi::rendering::details {

#ifdef __APPLE__
std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle);

details::ExtensionsNames getPlatformExtensionsInternal();
#endif

} // rendering::details
