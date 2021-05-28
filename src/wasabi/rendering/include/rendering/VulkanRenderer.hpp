
#pragma once

#include "rendering/Renderer.hpp"
#include "platform/WindowHandle.hpp"
#include "platform/Config.hpp"

#include <Vulkan/Vulkan.hpp>


namespace wasabi::rendering {

class VulkanRenderer {
public:
	VulkanRenderer(WindowHandle nativeHandle);
	~VulkanRenderer();
private:
	VkInstance m_instance;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkSurfaceKHR m_surface;
};

} // namespace wasabi::rendering
