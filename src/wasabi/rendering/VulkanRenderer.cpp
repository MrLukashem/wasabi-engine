
#include "rendering/VulkanRenderer.hpp"

#include "utils/FileUtils.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <optional>
#include <numeric>


namespace {

using namespace wasabi::rendering;

uint32_t meshHandleCounter = 1;

struct GPUMesh {
	struct MeshAlloc {
		uint32_t verticesOffset;
		uint32_t verticesCount;
		uint32_t verticesStride;
		uint32_t indicesOffset;
		uint32_t indicesCount;
	};

	VkBuffer verticesBuffer;
	VmaAllocation verticesAllocation;

	VkBuffer indicesBuffer;
	VmaAllocation indicesAllocation;

	std::unordered_map<MeshHandle, MeshAlloc> meshHandleToAllocationInfo;
};

auto logger = spdlog::stdout_color_mt("VulkanRenderer");

details::ExtensionsNames getPhysicalDeviceExtenions() {
	return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

details::SwapChainSetup chooseSwapChainSetup(const details::SwapChainDetails& details) {
	const auto& formatItr = std::find_if(
		details.formats.begin(),
		details.formats.end(),
		[] (const auto& format) {
			return format.format == VK_FORMAT_B8G8R8A8_SRGB
				&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		});
	const auto& modeItr = std::find_if(
		details.presentModes.begin(),
		details.presentModes.end(),
		[] (const auto& mode) {
			return mode == VK_PRESENT_MODE_MAILBOX_KHR;
		});
	const auto presentMode = modeItr != details.presentModes.end()
		? *modeItr : VK_PRESENT_MODE_FIFO_KHR;

	return {
		details.capabilities.currentExtent, details.capabilities,
		*formatItr, presentMode, details.capabilities.minImageCount + 1};
}

template <typename T>
T getOrThrow(const std::optional<T>& opt, const std::string& msg) {
	return opt.has_value() ? *opt : throw std::runtime_error(msg);
}

} // namespace

namespace wasabi::rendering {

VulkanRenderer::VulkanRenderer(const WindowHandle nativeHandle, const std::vector<ShaderInfo>& shaderInfos) {
	logger->info("ctr begin");

	if (const auto result = volkInitialize(); result != VK_SUCCESS) {
		logger->error("Failed to initialize volk: {}", details::vkResultToString(result));
	}

	m_instance = getOrThrow(
		details::createVkInstance(details::getPlatformExtensions()),
		"Error during vk instance creation");

	m_surface = getOrThrow(
		details::createVkSurface(m_instance, nativeHandle),
		"Error during vk surface creation");

	const auto &physicalDeviceOpt = details::getTheMostSuitableDevice(
		m_instance,
		[](const auto &device) {
			return details::supportsDeviceExtensions(device, getPhysicalDeviceExtenions());
		}
	);
	m_physicalDevice = getOrThrow(physicalDeviceOpt, "No device that meets criteria");

	createDevice();
	vkGetDeviceQueue(m_device, 0, 0, &m_graphicsQueue);
	createSwapChain();
	createImagesView();
	createRenderPass();
	createGraphicsPipeline(shaderInfos);
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
}

void VulkanRenderer::createDevice() {
	const auto& index = details::findQueueFamilyIndex(m_physicalDevice, m_surface);
	auto queueInfo = details::makeInfo<VkDeviceQueueCreateInfo>();
	queueInfo.queueFamilyIndex = *index;
	queueInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueInfo.pQueuePriorities = &queuePriority;

	const auto& extensions = getPhysicalDeviceExtenions();
	auto deviceInfo = details::makeInfo<VkDeviceCreateInfo>();
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	deviceInfo.ppEnabledExtensionNames = extensions.data();

	m_device = getOrThrow(
		details::Anvil<VkDevice>::forge(m_physicalDevice, deviceInfo),
		"Failed to create virtual device");
}

void VulkanRenderer::createSwapChain() {
	auto setup =
		chooseSwapChainSetup(details::querySwapChainDetails(m_physicalDevice, m_surface));

	auto info = details::makeInfo<VkSwapchainCreateInfoKHR>();
	info.surface = m_surface;
	info.minImageCount = setup.imageCount;
	info.imageFormat = setup.surfaceFormat.format;
	info.imageColorSpace = setup.surfaceFormat.colorSpace;
	info.imageExtent = setup.extent;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.preTransform = setup.capabilities.currentTransform;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode = setup.mode;
	info.clipped = VK_TRUE;
	info.oldSwapchain = VK_NULL_HANDLE;

	m_swapChain = getOrThrow(
		details::Anvil<VkSwapchainKHR>::forge(m_device, m_surface, info),
		"Failed to create swapchain");
	m_swapChainSetup = std::move(setup);
}

void VulkanRenderer::createImagesView() {
	const auto& info = [] (const auto& image, const auto& format) {
		auto info = details::makeInfo<VkImageViewCreateInfo>();
		info.image = image;
		info.format = format;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		return info;
	};

	const auto& images = details::getImages(m_device, m_swapChain);
	std::transform(
		images.begin(), images.end(), std::back_inserter(m_views),
		[this, &info, &format = m_swapChainSetup.surfaceFormat.format] (const auto& image) {
			return getOrThrow(
				details::Anvil<VkImageView>::forge(m_device, info(image, format)),
				"Failed to create ImageView");
		});
}

void VulkanRenderer::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainSetup.surfaceFormat.format; // to check it it is ok	
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	auto renderPassInfo = details::makeInfo<VkRenderPassCreateInfo>();
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	m_renderPass = getOrThrow(
		details::Anvil<VkRenderPass>::forge(m_device, renderPassInfo),
		"Failed to create a render pass");
}

void VulkanRenderer::createGraphicsPipeline(const std::vector<ShaderInfo> &shaderInfos) {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

	for (const auto &[stage, path]: shaderInfos) {
		auto module = getOrThrow(
			details::Anvil<VkShaderModule>::forge(m_device, utils::readFile(path)),
			"Failed to create a " + path + " module");
		auto shaderStageInfo = details::makeInfo<VkPipelineShaderStageCreateInfo>();
		shaderStageInfo.stage = details::toVkShaderStage(stage);
		shaderStageInfo.module = module;
		shaderStageInfo.pName = "main";

		shaderStages.push_back(shaderStageInfo);
	}

	auto vertexInputStateInfo = details::makeInfo<VkPipelineVertexInputStateCreateInfo>();
	auto inputAssemblyStateInfo = details::makeInfo<VkPipelineInputAssemblyStateCreateInfo>();
	inputAssemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewPort{};
	viewPort.height = m_swapChainSetup.extent.height;
	viewPort.width = m_swapChainSetup.extent.width;
	viewPort.minDepth = .0f;
	viewPort.maxDepth = 1.0f;

	VkRect2D rect2D;
	rect2D.extent = m_swapChainSetup.extent;
	rect2D.offset = {0, 0};

	auto viewPortStateInfo = details::makeInfo<VkPipelineViewportStateCreateInfo>();
	viewPortStateInfo.viewportCount = 1;
	viewPortStateInfo.pViewports = &viewPort;
	viewPortStateInfo.scissorCount = 1;
	viewPortStateInfo.pScissors = &rect2D;

	auto rasterizationStateInfo = details::makeInfo<VkPipelineRasterizationStateCreateInfo>();
	rasterizationStateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateInfo.lineWidth = 1.0f;
	rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateInfo.depthBiasEnable = VK_FALSE;

	auto multisamplesStateInfo = details::makeInfo<VkPipelineMultisampleStateCreateInfo>();
	multisamplesStateInfo.sampleShadingEnable = VK_FALSE;
	multisamplesStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplesStateInfo.minSampleShading = 1.0f;
	multisamplesStateInfo.pSampleMask = nullptr;
	multisamplesStateInfo.alphaToCoverageEnable = VK_FALSE;
	multisamplesStateInfo.alphaToOneEnable = VK_FALSE;

	
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	auto colorBlendStateInfo = details::makeInfo<VkPipelineColorBlendStateCreateInfo>();
	colorBlendStateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateInfo.attachmentCount = 1;
	colorBlendStateInfo.pAttachments = &colorBlendAttachment;

	auto pipelineLayoutInfo = details::makeInfo<VkPipelineLayoutCreateInfo>();
	m_pipelineLayout = getOrThrow(
		details::Anvil<VkPipelineLayout>::forge(m_device, pipelineLayoutInfo),
		"Failed to create pipeline layout");

	auto pipelineInfo = details::makeInfo<VkGraphicsPipelineCreateInfo>();
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputStateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
	pipelineInfo.pViewportState = &viewPortStateInfo;
	pipelineInfo.pRasterizationState = &rasterizationStateInfo;
	pipelineInfo.pMultisampleState = &multisamplesStateInfo;
	pipelineInfo.pColorBlendState = &colorBlendStateInfo;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	m_pipeline = getOrThrow(
		details::Anvil<VkPipeline>::forge(m_device, pipelineInfo),
		"Failed to create Pipeline");

	for (auto & shaderStage : shaderStages) {
		vkDestroyShaderModule(m_device, shaderStage.module, nullptr);
	}
}

void VulkanRenderer::createFramebuffers() {
	for (const auto & view : m_views) {
		const VkImageView attachments[] = {
			view
		};

		auto framebufferInfo = details::makeInfo<VkFramebufferCreateInfo>();
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &attachments[0];
		framebufferInfo.width = m_swapChainSetup.extent.width;
		framebufferInfo.height = m_swapChainSetup.extent.height;
		framebufferInfo.layers = 1;

		m_framebuffers.push_back(getOrThrow(
			details::Anvil<VkFramebuffer>::forge(m_device, framebufferInfo),
			"Failed to create Framebuffer"));
	}
}

void VulkanRenderer::createCommandPool() {
	const auto queueFamilyIndex = details::findQueueFamilyIndex(m_physicalDevice, m_surface);
	if (!queueFamilyIndex.has_value()) {
		return;
	}

	auto poolInfo = details::makeInfo<VkCommandPoolCreateInfo>();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndex.value();

	m_commandPool = getOrThrow(
		details::Anvil<VkCommandPool>::forge(m_device, poolInfo),
		"Failed to create CommandPool");
}

void VulkanRenderer::createCommandBuffers() {
	auto commandBufferInfo = details::makeInfo<VkCommandBufferAllocateInfo>();
	commandBufferInfo.commandPool = m_commandPool;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandBufferCount = 1;

	m_commandBuffer = getOrThrow(
		details::Anvil<VkCommandBuffer>::forge(m_device, commandBufferInfo),
		"Failed to create CommandBuffer");
}

void VulkanRenderer::createSyncObjects() {
	const auto semaphoreInfo = details::makeInfo<VkSemaphoreCreateInfo>();
	auto fenceInfo = details::makeInfo<VkFenceCreateInfo>();
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	m_imageAvailableSemaphore = getOrThrow(
		details::Anvil<VkSemaphore>::forge(m_device, semaphoreInfo),
		"Failed to create imageAvailableSemaphore");
	m_renderFinishedSemaphore = getOrThrow(
		details::Anvil<VkSemaphore>::forge(m_device, semaphoreInfo),
		"Failed to create renderFinishedSemaphore");
	m_inFlightFence = getOrThrow(
		details::Anvil<VkFence>::forge(m_device, fenceInfo),
		"Failed to create inFlightFence");
}

void VulkanRenderer::recordCommandBuffer(const uint32_t imageIndex) const {
	if (imageIndex >= m_framebuffers.size()) {
		throw std::runtime_error("Invalid image index");
	}

	auto beginInfo = details::makeInfo<VkCommandBufferBeginInfo>();
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	auto recordInfo = details::makeInfo<VkRenderPassBeginInfo>();
	recordInfo.renderPass = m_renderPass;
	recordInfo.framebuffer = m_framebuffers[imageIndex];
	recordInfo.renderArea.offset = {0, 0};
	recordInfo.renderArea.extent = m_swapChainSetup.extent;

	constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	recordInfo.clearValueCount = 1;
	recordInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_commandBuffer, &recordInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapChainSetup.extent.width);
	viewport.height = static_cast<float>(m_swapChainSetup.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = m_swapChainSetup.extent;
	vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

	vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(m_commandBuffer);

	if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to end recording command buffer");
	}
}

VulkanRenderer::~VulkanRenderer() {
	vkDestroyPipeline(m_device, m_pipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	for (const auto& view : m_views) {
		vkDestroyImageView(m_device, view, nullptr);
	}
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
	vkDestroyDevice(m_device, nullptr);
	vkDestroyInstance(m_instance, nullptr);
	for (const auto& framebuffer : m_framebuffers) {
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}
	if (m_commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}
	vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
	vkDestroyFence(m_device, m_inFlightFence, nullptr);
}

void VulkanRenderer::drawFrame() const {
	vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(m_device, 1, &m_inFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(
		m_device,
		m_swapChain,
		std::numeric_limits<uint64_t>::max(),
		m_imageAvailableSemaphore,
		VK_NULL_HANDLE,
		&imageIndex);

	vkResetCommandBuffer(m_commandBuffer, 0);
	recordCommandBuffer(imageIndex);

	auto submitInfo = details::makeInfo<VkSubmitInfo>();
	const VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphore};
	constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffer;

	const VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	auto presentInfo = details::makeInfo<VkPresentInfoKHR>();
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	const VkSwapchainKHR swapChains[] = {m_swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(m_graphicsQueue, &presentInfo);
}

VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

void endSingleTimeCommands(VkDevice device, VkQueue queue,
						   VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

std::optional<MeshHandle> VulkanRenderer::uploadMeshBatch(const std::vector<Mesh> &meshes) {
	const std::size_t bufferSize = std::reduce(
	meshes.begin(),
	meshes.end(),
	0,
	[] (const std::size_t result, const Mesh& rh) {
		return rh.vertices.size() * sizeof(Vertex)
			+ rh.indices.size() * sizeof(uint32_t)
			+ result;
	});

	VkBuffer stagingBuffer;
	auto stagingBufferCreateInfo = details::makeInfo<VkBufferCreateInfo>();
	stagingBufferCreateInfo.size = bufferSize;
	stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocator stagingAllocator{};
	VmaAllocation stagingBufferAllocation;
	VmaAllocationInfo stagingBufferInfo;
	VmaAllocationCreateInfo stagingBufferAllocInfo{};
	stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	vmaCreateBuffer(
		stagingAllocator,
		&stagingBufferCreateInfo,
		&stagingBufferAllocInfo,
		&stagingBuffer,
		&stagingBufferAllocation,
		&stagingBufferInfo);

	void* mappedData;
	vmaMapMemory(stagingAllocator, stagingBufferAllocation, &mappedData);
	memccpy(mappedData, meshes.data(), 0, bufferSize);
	vmaUnmapMemory(stagingAllocator, stagingBufferAllocation);

	VkBuffer buffer;
	auto bufferCreateInfo = details::makeInfo<VkBufferCreateInfo>();
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	const VmaAllocator allocator{};
	VmaAllocation bufferAllocation;
	VmaAllocationInfo bufferInfo;
	VmaAllocationCreateInfo bufferAllocInfo{};
	bufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	vmaCreateBuffer(
		allocator,
		&bufferCreateInfo,
		&bufferAllocInfo,
		&buffer,
		&bufferAllocation,
		&bufferInfo);

	const auto commandBuffer = beginSingleTimeCommands(m_device, m_commandPool);

	VkBufferCopy copyRegion{};
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &copyRegion);

	endSingleTimeCommands(m_device, m_graphicsQueue, m_commandPool, commandBuffer);

	vmaDestroyBuffer(stagingAllocator, stagingBuffer, stagingBufferAllocation);

	GPUMesh gpuMesh{};
	gpuMesh.verticesBuffer = buffer;
	gpuMesh.verticesAllocation = bufferAllocation;

	uint32_t verticesOffset = 0;
	uint32_t indicesOffset = 0;
	for (const auto& mesh : meshes) {
		GPUMesh::MeshAlloc meshAlloc {
			.verticesCount = static_cast<uint32_t>(mesh.vertices.size()),
			.verticesOffset = verticesOffset,
			.verticesStride = sizeof(Vertex),
			.indicesCount =	static_cast<uint32_t>(mesh.indices.size()),
			.indicesOffset = indicesOffset
		};

		verticesOffset += verticesOffset + meshAlloc.verticesCount * meshAlloc.verticesStride;
		indicesOffset += indicesOffset + meshAlloc.indicesCount * sizeof(uint32_t);

		MeshHandle handle{meshHandleCounter++};
		gpuMesh.meshHandleToAllocationInfo[handle] = meshAlloc;
	}
}

} // namespace wasabi::rendering
