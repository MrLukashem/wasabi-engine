
#include "rendering/VulkanRenderer.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace {

auto logger = spdlog::stdout_color_mt("VulkanRenderer");

} // namespace

namespace wasabi::rendering {

VulkanRenderer::VulkanRenderer() : m_instance{} {
	logger->info("ctr begin");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "WasabiEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "WasabiEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;

	auto result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	if (result != VK_SUCCESS) {
		logger->info("Failed to create vkinstance");
	}

	uint32_t deviceCount{};
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
	if (deviceCount == 0) {
		throw std::runtime_error("No physical devices supported");
		return;
	}

	for (const auto& device : devices) {
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(device, &properties);
		logger->info(properties.deviceName);
	}
}

VulkanRenderer::~VulkanRenderer() {
	vkDestroyInstance(m_instance, nullptr);
}

} // namespace wasabi::rendering
