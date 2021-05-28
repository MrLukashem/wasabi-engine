
#include "rendering/VulkanRenderer.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <windows.h>


namespace {

auto logger = spdlog::stdout_color_mt("VulkanRenderer");

} // namespace

namespace wasabi::rendering {

VulkanRenderer::VulkanRenderer(WindowHandle nativeHandle) : m_instance{} {
	logger->info("ctr begin");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "WasabiEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "WasabiEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	// EXTENSIONS QUERY
	uint32_t propertiesCount{};
	vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
	if (propertiesCount == 0) {
		throw std::runtime_error("No Extensions for physical device");
		return;
	}

	std::vector<VkExtensionProperties> extensions(propertiesCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensions.data());
	std::vector<char*> wantedExtentioins{};
	for (auto extension : extensions) {
		if (std::strcmp(extension.extensionName, VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0) {
			logger->info("VK_KHR_win32_surface found!!!");
			wantedExtentioins.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		}
		if (std::strcmp(extension.extensionName, VK_KHR_SURFACE_EXTENSION_NAME) == 0) {
			logger->info("VK_KHR_surface found!!!");
			wantedExtentioins.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		}
		if (std::strcmp(extension.extensionName, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0) {
			logger->info("VK_KHR_surface found!!!");
			wantedExtentioins.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		logger->info(extension.extensionName);
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.enabledExtensionCount = wantedExtentioins.size();
	createInfo.ppEnabledExtensionNames = wantedExtentioins.data();

	auto result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	if (result != VK_SUCCESS) {
		logger->info("Failed to create vkinstance");
	}

	// Surface attach
	VkWin32SurfaceCreateInfoKHR r{};
	r.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	r.hinstance = GetModuleHandle(nullptr);
	r.hwnd = nativeHandle;
	//r.pNext = nullptr;

	if (vkCreateWin32SurfaceKHR(m_instance, &r, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
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

	m_physicalDevice = devices.front();




	uint32_t queueFamilyCount{};
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
	if (queueFamilyCount == 0) {
		throw std::runtime_error("No Queue Families avaiable for the device");
		return;
	}

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

	const auto& queueFamilyItr = std::find_if(queueFamilies.begin(), queueFamilies.end(),
		[](const auto& queue) {
			return queue.queueFlags & VK_QUEUE_GRAPHICS_BIT;
		});

	if (queueFamilyItr == queueFamilies.end()) {
		throw std::runtime_error("No Graphics queue");
		return;
	}

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_device, 0, 0, &m_graphicsQueue);
}

VulkanRenderer::~VulkanRenderer() {
	vkDestroyDevice(m_device, nullptr);
	vkDestroyInstance(m_instance, nullptr);
}

} // namespace wasabi::rendering
