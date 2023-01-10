//
// Created by Łukasz Merta on 27/11/2021.
//

#pragma once

#include "platform/WindowHandle.hpp"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>


namespace wasabi::rendering::details {
    using ExtensionsNames = std::vector<const char *>;
}

namespace wasabi::rendering::details::macos {

#ifdef __APPLE__
//std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle);
//
//details::ExtensionsNames getPlatformExtensionsInternal();
#endif

} // rendering::details::macos
