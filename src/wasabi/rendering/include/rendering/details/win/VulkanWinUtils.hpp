//
// Created by ryuu on 24.10.2021.
//

#pragma once

#include "platform/WindowHandle.hpp"

#include <vulkan/vulkan.h>

#include <optional>


namespace wasabi::rendering::details::win {

#ifdef _WIN32
std::optional<VkSurfaceKHR> createVkSurfaceIntenral(VkInstance instance, WindowHandle nativeHandle);

details::ExtensionsNames getPlatformExtensionsInternal();
#endif

} // wasabi::rendering::details::win
