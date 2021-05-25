
#pragma once

#include "rendering/Renderer.hpp"

#include <Vulkan/Vulkan.hpp>


namespace wasabi::rendering {

class VulkanRenderer {
public:
	VulkanRenderer();
	virtual ~VulkanRenderer();
private:
	VkInstance m_instance;
};

} // namespace wasabi::rendering
