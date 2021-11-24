
#include "rendering/details/VulkanUtils.hpp"

#include "rendering/details/linux/VulkanLinuxUtils.hpp"
#include "rendering/details/win/VulkanWinUtils.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <limits>


namespace {

auto logger = spdlog::stdout_color_mt("VulkanUtils");

} // namespace

namespace wasabi::rendering::details {

using namespace lx;
using namespace win;

std::optional<uint32_t> findQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
	uint32_t queueFamilyCount{};
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	uint32_t queueFamilyIndex{};
	VkBool32 presentSupported = VK_FALSE;
	for (const auto& queue : queueFamilies) {
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &presentSupported);
		if ((queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) && presentSupported) {
			return queueFamilyIndex;
		}

		queueFamilyIndex++;
	}

	return {};
}

SwapChainDetails querySwapChainDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
	SwapChainDetails details{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatsCount{};
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, nullptr);
	details.formats.resize(formatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, details.formats.data());

	uint32_t modesCount{};
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modesCount, nullptr);
	details.presentModes.resize(modesCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modesCount, details.presentModes.data());

	return details;
}

ExtensionsNames getPlatformExtensions() {
    return getPlatformExtensionsInternal();
}

std::vector<const char*> getNotAvailableExtensions(
	const std::vector<VkExtensionProperties>& supportedExtensions,
	const ExtensionsNames& requestedExtensionsNames
) {
	std::vector<const char*> notAvailableExtensions{};
	for (const auto& extension : supportedExtensions) {
		if (std::find_if(requestedExtensionsNames.begin(), requestedExtensionsNames.end(),
			[&extension] (const auto requestedExtension)
			{
				return std::strcmp(requestedExtension, extension.extensionName) == 0;
			}) == requestedExtensionsNames.end()) {

			notAvailableExtensions.emplace_back(extension.extensionName);
		}
	}

	return notAvailableExtensions;
}

bool supportsInstanceExtensions(const ExtensionsNames& requestedExtensions) {
	uint32_t extensionsCount{};
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

    const auto& notAvailableExtensions = getNotAvailableExtensions(extensions, requestedExtensions);
    if (!notAvailableExtensions.empty()) {
        logger->warn("Some extensions are not supported:");
        for (const auto& ext : getNotAvailableExtensions(extensions, requestedExtensions)) {
            logger->warn(ext);
        }

        return false;
    }

    return true;
}

bool supportsDeviceExtensions(
	const VkPhysicalDevice physicalDevice,
	const ExtensionsNames& requestedExtensions) {
	uint32_t extensionsCount{};
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionsCount);
	const auto vkResult = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, extensions.data());

	return vkResult == VK_SUCCESS;
}

std::optional<VkInstance> createVkInstance(const ExtensionsNames& requiredExtensions) {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "WasabiEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "WasabiEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	if (!supportsInstanceExtensions(requiredExtensions)) {
		logger->error("Vulkan does not support all required extensions!");
		return {};
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VkInstance vkInstance;
	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		logger->error("Failed to create vkinstance");
		return {};
	}

	return vkInstance;
}

std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle) {
    return details::lx::createVkSurface(instance, nativeHandle);
}

std::optional<VkPhysicalDevice> getTheMostSuitableDevice(
	const VkInstance instance,
	const std::function<uint32_t(VkPhysicalDevice)>& rateFunction
) {
	uint32_t devicesCount{};
	vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);

	std::vector<VkPhysicalDevice> devices(devicesCount);
	vkEnumeratePhysicalDevices(instance, &devicesCount, devices.data());

	auto theBestRate = std::numeric_limits<uint32_t>::min();
	std::optional<VkPhysicalDevice> result{};
	VkPhysicalDeviceProperties properties{};
	for (const auto& device : devices) {
		const auto rate = rateFunction(device);
		if (rate >= theBestRate) {
			theBestRate = rate;
			result = device;
		}
	}

	return result;
}

std::optional<VkDevice> createVkDevice(
	VkPhysicalDevice physicalDevice,
	VkDeviceCreateInfo info
) {
	VkDevice device{};
	VkPhysicalDeviceFeatures deviceFeatures{};
	info.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(physicalDevice, &info, nullptr, &device) != VK_SUCCESS) {
		 logger->error("failed to create logical device!");
		 return {};
	}

	return device;
}

std::optional<VkSwapchainKHR> createSwapchain(
	VkDevice device,
	VkSurfaceKHR surface,
	const VkSwapchainCreateInfoKHR& info
) {
	VkSwapchainKHR swapChain;
	if (vkCreateSwapchainKHR(device, &info, nullptr, &swapChain) != VK_SUCCESS) {
		logger->error("Failed to create swapchain");
		return {};
	}

	return swapChain;
}

std::vector<VkImage> getImages(VkDevice device, VkSwapchainKHR swapChain) {
	uint32_t imageCount{};
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	std::vector<VkImage> images(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

	return images;
}

std::optional<VkImageView> createImageView(VkDevice device, const VkImageViewCreateInfo& info) {
	VkImageView imageView{};
	if (vkCreateImageView(device, &info, nullptr, &imageView) != VK_SUCCESS) {
		logger->error("Failed to create ImageView");
		return {};
	}

	return imageView;
}

std::optional<VkShaderModule> createShaderModule(VkDevice device, const utils::Bytes& spirv) {
	auto info = makeInfo<VkShaderModuleCreateInfo>();
	info.codeSize = spirv.size();
	info.pCode = reinterpret_cast<const uint32_t*>(spirv.data());

	VkShaderModule module{};
	if (vkCreateShaderModule(device, &info, nullptr, &module) != VK_SUCCESS) {
		return {};
	}

	return module;
}

std::optional<VkPipelineLayout> createPipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& info) {
	VkPipelineLayout layout{};
	if (vkCreatePipelineLayout(device, &info, nullptr, &layout) != VK_SUCCESS) {
		return {};
	}

	return layout;
}

std::optional<VkRenderPass> createRenderPass(VkDevice device, const VkRenderPassCreateInfo& info) {
	VkRenderPass renderPass{};
	if (vkCreateRenderPass(device, &info, nullptr, &renderPass) != VK_SUCCESS) {
		return {};
	}

	return renderPass;
}


std::optional<VkPipeline> createPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& info) {
	VkPipeline pipeline{};
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline)) {
		return {};
	}

	return pipeline;
}

template <>
VkSwapchainCreateInfoKHR makeInfo<VkSwapchainCreateInfoKHR>() {
	VkSwapchainCreateInfoKHR info{};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	return info;
}

template <>
VkImageViewCreateInfo makeInfo<VkImageViewCreateInfo>() {
	VkImageViewCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	return info;
}

template <>
VkDeviceCreateInfo makeInfo<VkDeviceCreateInfo>() {
	VkDeviceCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	return info;
}

template <>
VkDeviceQueueCreateInfo makeInfo<VkDeviceQueueCreateInfo>() {
	VkDeviceQueueCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

	return info;
}

template <>
VkShaderModuleCreateInfo makeInfo<VkShaderModuleCreateInfo>() {
	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	return info;
}

template <>
VkPipelineShaderStageCreateInfo makeInfo<VkPipelineShaderStageCreateInfo>() {
	VkPipelineShaderStageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	return info;
}

template <>
VkPipelineVertexInputStateCreateInfo makeInfo<VkPipelineVertexInputStateCreateInfo>() {
	VkPipelineVertexInputStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	return info;
}

template <>
VkPipelineInputAssemblyStateCreateInfo makeInfo<VkPipelineInputAssemblyStateCreateInfo>() {
	VkPipelineInputAssemblyStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

	return info;
}

template <>
VkPipelineViewportStateCreateInfo makeInfo<VkPipelineViewportStateCreateInfo>() {
	VkPipelineViewportStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

	return info;
}

template <>
VkPipelineRasterizationStateCreateInfo makeInfo<VkPipelineRasterizationStateCreateInfo>() {
	VkPipelineRasterizationStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

	return info;
}

template <>
VkPipelineMultisampleStateCreateInfo makeInfo<VkPipelineMultisampleStateCreateInfo>() {
	VkPipelineMultisampleStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

	return info;
}

template <>
VkPipelineColorBlendStateCreateInfo makeInfo<VkPipelineColorBlendStateCreateInfo>() {
	VkPipelineColorBlendStateCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	return info;
}

template <>
VkPipelineLayoutCreateInfo makeInfo<VkPipelineLayoutCreateInfo>() {
	VkPipelineLayoutCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	return info;
}

template <>
VkRenderPassCreateInfo makeInfo<VkRenderPassCreateInfo>() {
	VkRenderPassCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	return info;
}

template <>
VkGraphicsPipelineCreateInfo makeInfo<VkGraphicsPipelineCreateInfo>() {
	VkGraphicsPipelineCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	return info;
}

} // namespace wasabi::rendering::details

