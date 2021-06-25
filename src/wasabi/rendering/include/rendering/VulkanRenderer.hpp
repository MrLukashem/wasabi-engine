
#pragma once

#include "rendering/Renderer.hpp"
#include "rendering/details/VulkanUtils.hpp"


namespace wasabi::rendering {

class VulkanRenderer {
public:
	VulkanRenderer(WindowHandle nativeHandle);
	~VulkanRenderer();
private:
	void createDevice();
	void createSwapChain();
	void createImagesView();
	void createRenderPass();
	void createGraphicsPipeline();

	VkInstance m_instance;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapChain;
	details::SwapChainSetup m_swapChainSetup;
	std::vector<VkImageView> m_views;
	VkRenderPass m_renderPass;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
};

} // namespace wasabi::rendering
