
#pragma once

#include "Math.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Shader.h"
#include "rendering/details/VulkanUtils.hpp"


namespace wasabi::rendering {

enum class MeshHandle: uint32_t { kInvalid = 0 };

class VulkanRenderer {
public:
	explicit VulkanRenderer(WindowHandle nativeHandle, const std::vector<ShaderInfo>& shaderInfos);
	~VulkanRenderer();

	void drawFrame() const;

	std::optional<MeshHandle> uploadMeshBatch(const std::vector<Mesh>& meshes);
private:
	void createDevice();
	void createSwapChain();
	void createImagesView();
	void createRenderPass();
	void createGraphicsPipeline(const std::vector<ShaderInfo> &shaderInfos);
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();

	void recordCommandBuffer(uint32_t imageIndex) const;

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
	std::vector<VkFramebuffer> m_framebuffers;
	VkCommandPool m_commandPool;
	VkCommandBuffer m_commandBuffer;
	VkSemaphore m_imageAvailableSemaphore;
	VkSemaphore m_renderFinishedSemaphore;
	VkFence m_inFlightFence;
};

} // namespace wasabi::rendering
