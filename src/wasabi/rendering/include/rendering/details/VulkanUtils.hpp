
#pragma once

#include "rendering/Shader.h"
#include "platform/Config.hpp"
#include "platform/WindowHandle.hpp"
#include "utils/defs.hpp"

// #include <vulkan/vulkan.h>
#define VK_USE_PLATFORM_METAL_EXT
#define VK_USE_PLATFORM_MACOS_MVK
#include <volk.h>

#include <algorithm>
#include <functional>
#include <optional>
#include <vector>
#include <type_traits>


namespace wasabi::rendering::details {

using ExtensionsNames = std::vector<const char*>;

struct SwapChainDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainSetup {
	VkExtent2D extent;
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR mode;
	uint32_t imageCount;
};

std::string_view vkResultToString(VkResult result) noexcept;

VkShaderStageFlagBits toVkShaderStage(const ShaderStage stage);

std::optional<uint32_t> findQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

SwapChainDetails querySwapChainDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

ExtensionsNames getPlatformExtensions();

std::vector<const char*> getAvailableExtensions(
	const std::vector<VkExtensionProperties>& supportedExtensions,
	const ExtensionsNames& requestedExtensionsNames);

bool supportsInstanceExtensions(const ExtensionsNames& requestedExtensions);

bool supportsDeviceExtensions(
	const VkPhysicalDevice physicalDevice,
	const ExtensionsNames& requestedExtensions);

std::optional<VkInstance> createVkInstance(const ExtensionsNames& requiredExtensions);

std::optional<VkSurfaceKHR> createVkSurface(VkInstance instance, WindowHandle nativeHandle);

std::optional<VkPhysicalDevice> getTheMostSuitableDevice(
	const VkInstance instance,
	const std::function<uint32_t(VkPhysicalDevice)>& rateFunction);


std::optional<VkDevice> createVkDevice(
	VkPhysicalDevice physicalDevice,
	VkDeviceCreateInfo info);

std::optional<VkSwapchainKHR> createSwapchain(
	VkDevice device,
	VkSurfaceKHR surface,
	const VkSwapchainCreateInfoKHR& info);

std::vector<VkImage> getImages(VkDevice device, VkSwapchainKHR swapChain);

std::optional<VkImageView> createImageView(VkDevice device, const VkImageViewCreateInfo& info);

std::optional<VkShaderModule> createShaderModule(VkDevice device, const utils::Bytes& spirv);

std::optional<VkPipelineLayout> createPipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& info);

std::optional<VkRenderPass> createRenderPass(VkDevice device, const VkRenderPassCreateInfo& info);

std::optional<VkPipeline> createPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& info);

std::optional<VkFramebuffer> createFrameBuffer(VkDevice device, const VkFramebufferCreateInfo& info);

std::optional<VkCommandPool> createCommandPool(VkDevice device, const VkCommandPoolCreateInfo& info);

std::optional<VkCommandBuffer> createCommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo& info);

std::optional<VkSemaphore> createSemaphore(VkDevice device, const VkSemaphoreCreateInfo& info);

std::optional<VkFence> createFence(VkDevice device, const VkFenceCreateInfo& info);

template <typename VK_T, typename... Args>
std::optional<VK_T> create(Args&&... args) {
	return {};
}

template <typename... Args>
std::optional<VkImageView> create(Args&&... args) {
	return createImageView(args...);
}

template <typename VK_T, typename... Args>
std::optional<VK_T> forge(Args... args) {
	return {};
}

// for further use
template <typename VK_T, typename = std::enable_if<std::is_pointer_v<VK_T>>>
struct VkObject {
	using VkDeleter = std::function<void(VK_T& object)>;

	VkObject() : m_object{nullptr}, m_deleter{} {}

	VkObject(VK_T& object, VkDeleter&& deleter)
		: m_object{object}, m_deleter{std::move(deleter)} {}

	~VkObject() {
		m_deleter(m_object);
	}
	
	operator VK_T() const noexcept {
		return m_object;
	}

	operator bool() const noexcept {
		return m_object != nullptr;
	}
private:
	VK_T m_object;
	VkDeleter m_deleter;
};

template <typename VK_T>
struct Anvil {
	template <typename... Args>
	static std::optional<VK_T> forge(Args... args) {
		return {};
	}
};

template <>
template <typename... Args>
std::optional<VkSwapchainKHR> Anvil<VkSwapchainKHR>::forge(Args... args) {
	return createSwapchain(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkImageView> Anvil<VkImageView>::forge(Args... args) {
	return createImageView(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkDevice> Anvil<VkDevice>::forge(Args... args) {
	return createVkDevice(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkShaderModule> Anvil<VkShaderModule>::forge(Args... args) {
	return createShaderModule(std::forward<Args>(args)...);
}

template<>
template <typename... Args>
std::optional<VkPipelineLayout> Anvil<VkPipelineLayout>::forge(Args... args) {
	return createPipelineLayout(std::forward<Args>(args)...);
}

template<>
template <typename... Args>
std::optional<VkRenderPass> Anvil<VkRenderPass>::forge(Args... args) {
	return createRenderPass(std::forward<Args>(args)...);
}

template<>
template <typename... Args>
std::optional<VkPipeline> Anvil<VkPipeline>::forge(Args... args) {
	return createPipeline(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkFramebuffer> Anvil<VkFramebuffer>::forge(Args... args) {
	return createFrameBuffer(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkCommandPool> Anvil<VkCommandPool>::forge(Args... args) {
	return createCommandPool(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkCommandBuffer> Anvil<VkCommandBuffer>::forge(Args... args) {
	return createCommandBuffer(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkSemaphore> Anvil<VkSemaphore>::forge(Args... args) {
	return createSemaphore(std::forward<Args>(args)...);
}

template <>
template <typename... Args>
std::optional<VkFence> Anvil<VkFence>::forge(Args... args) {
	return createFence(std::forward<Args>(args)...);
}

template <typename T>
T makeInfo() {
	return {};
}

template <>
VkSwapchainCreateInfoKHR makeInfo<VkSwapchainCreateInfoKHR>();

template <>
VkImageViewCreateInfo makeInfo<VkImageViewCreateInfo>();

template <>
VkDeviceCreateInfo makeInfo<VkDeviceCreateInfo>();

template <>
VkDeviceQueueCreateInfo makeInfo<VkDeviceQueueCreateInfo>();

template <>
VkShaderModuleCreateInfo makeInfo<VkShaderModuleCreateInfo>();

template <>
VkPipelineShaderStageCreateInfo makeInfo<VkPipelineShaderStageCreateInfo>();

template <>
VkPipelineVertexInputStateCreateInfo makeInfo<VkPipelineVertexInputStateCreateInfo>();

template <>
VkPipelineInputAssemblyStateCreateInfo makeInfo<VkPipelineInputAssemblyStateCreateInfo>();

template <>
VkPipelineViewportStateCreateInfo makeInfo<VkPipelineViewportStateCreateInfo>();

template <>
VkPipelineRasterizationStateCreateInfo makeInfo<VkPipelineRasterizationStateCreateInfo>();

template <>
VkPipelineMultisampleStateCreateInfo makeInfo<VkPipelineMultisampleStateCreateInfo>();

template <>
VkPipelineColorBlendStateCreateInfo makeInfo<VkPipelineColorBlendStateCreateInfo>();

template <>
VkPipelineLayoutCreateInfo makeInfo<VkPipelineLayoutCreateInfo>();

template <>
VkRenderPassCreateInfo makeInfo<VkRenderPassCreateInfo>();

template <>
VkGraphicsPipelineCreateInfo makeInfo<VkGraphicsPipelineCreateInfo>();

template <>
VkCommandPoolCreateInfo makeInfo<VkCommandPoolCreateInfo>();

template <>
VkCommandBufferAllocateInfo makeInfo<VkCommandBufferAllocateInfo>();

template <>
VkCommandBufferBeginInfo makeInfo<VkCommandBufferBeginInfo>();

template <>
VkRenderPassBeginInfo makeInfo<VkRenderPassBeginInfo>();

template <>
VkSemaphoreCreateInfo makeInfo<VkSemaphoreCreateInfo>();

template <>
VkFenceCreateInfo makeInfo<VkFenceCreateInfo>();

template <>
VkSubmitInfo makeInfo<VkSubmitInfo>();

template <>
VkPresentInfoKHR makeInfo<VkPresentInfoKHR>();

} // namespace wasabi::rendering::details
