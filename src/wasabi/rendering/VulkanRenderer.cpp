
#include "rendering/VulkanRenderer.hpp"

#include "utils/FileUtils.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <optional>


namespace {

using namespace wasabi::rendering;

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

VulkanRenderer::VulkanRenderer(WindowHandle nativeHandle) {
    logger->info("ctr begin");

    m_instance = getOrThrow(
        details::createVkInstance(details::getPlatformExtensions()),
        "Error during vk instance creation");

    m_surface = getOrThrow(
        details::createVkSurface(m_instance, nativeHandle),
        "Error during vk surface creation");

    const auto &physicalDeviceOpt = details::getTheMostSuitableDevice(m_instance,
                                                                      [](const auto &device) {
                                                                          return details::supportsDeviceExtensions(
                                                                              device, getPhysicalDeviceExtenions());
                                                                      });
    m_physicalDevice = getOrThrow(physicalDeviceOpt, "No device that meets criteria");

    createDevice();
    vkGetDeviceQueue(m_device, 0, 0, &m_graphicsQueue);
    createSwapChain();
    createImagesView();
    createRenderPass();
    createGraphicsPipeline();
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

	auto renderPassInfo = details::makeInfo<VkRenderPassCreateInfo>();
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	m_renderPass = getOrThrow(
		details::Anvil<VkRenderPass>::forge(m_device, renderPassInfo),
		"Failed to create a render pass");
}

void VulkanRenderer::createGraphicsPipeline() {
	auto vertModule = getOrThrow(
		details::Anvil<VkShaderModule>::forge(m_device, utils::readFile("C://VulkanSDK//1.2.170.0//Bin//vert.spv")),
		"Failed to create a vertex shader module");
	auto fragModule = getOrThrow(
		details::Anvil<VkShaderModule>::forge(m_device, utils::readFile("C://VulkanSDK//1.2.170.0//Bin//frag.spv")),
		"Failed to create a fragment shader module");

	auto vertexShaderStageinfo = details::makeInfo<VkPipelineShaderStageCreateInfo>();
	vertexShaderStageinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageinfo.module = vertModule;
	vertexShaderStageinfo.pName = "main";

	auto fragmentShaderStageInfo = details::makeInfo<VkPipelineShaderStageCreateInfo>();
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = fragModule;
	fragmentShaderStageInfo.pName = "main";

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
		vertexShaderStageinfo,
		fragmentShaderStageInfo
	};

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
	pipelineInfo.stageCount = 2;
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

	vkDestroyShaderModule(m_device, vertModule, nullptr);
	vkDestroyShaderModule(m_device, fragModule, nullptr);
}

VulkanRenderer::~VulkanRenderer() {
	vkDestroyPipeline(m_device, m_pipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	for (auto& view : m_views) {
		vkDestroyImageView(m_device, view, nullptr);
	}
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
	vkDestroyDevice(m_device, nullptr);
	vkDestroyInstance(m_instance, nullptr);
}

} // namespace wasabi::rendering
