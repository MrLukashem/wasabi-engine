//
// Created by ryuu on 24.10.2021.
//

#pragma once

#include "platform/WindowHandle.hpp"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>


namespace wasabi::rendering::details {
using ExtensionsNames = std::vector<const char *>;
}

namespace wasabi::rendering::details::lx {

#ifdef __linux__
std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle);

details::ExtensionsNames getPlatformExtensionsInternal();
#endif

} // rendering::details::lx
