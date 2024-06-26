//
// Created by sebastian on 09.06.24.
//

#include "VulkanDevice.hpp"

namespace Graphics::Vulkan {
	VulkanDevice::VulkanDevice() {
		m_physicalDevice = VK_NULL_HANDLE;
		m_device = VK_NULL_HANDLE;
		m_graphicsFamily = std::nullopt;
		m_presentFamily = std::nullopt;
	}

	VulkanDevice::~VulkanDevice() = default;

	void VulkanDevice::CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface>& surface) {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPU that supports vulkan!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device: devices) {
			if (IsPhysicalDeviceSuitable(device, surface)) {
				m_physicalDevice = device;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable gpu");
	}

	void VulkanDevice::CreateLogicalDevice() {
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {m_graphicsFamily.value(), m_presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily: uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
		createInfo.enabledLayerCount = 0;

		VkDevice device;
		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &device) != VK_FALSE) {
			throw std::runtime_error("Failed to create logical device");
		}
		m_device = std::make_optional(device);
	}

	void VulkanDevice::Release() {
		vkDestroyDevice(m_device.value(), nullptr);
	}

	VkDevice VulkanDevice::GetLogicalDevice() const {
		if (m_device.has_value()) { return m_device.value(); }
		throw std::runtime_error("Device is not initialized yet!");
	}

	uint32_t VulkanDevice::GetMaxUsableSampleCount() {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
		                            physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return 64; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return 32; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return 16; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return 8; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return 4; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return 2; }

		return 1;
	}

	bool VulkanDevice::IsFormatSupported(Graphics::PixelFormat pixelFormat) {
		VkFormat vkPixelFormat = pixelFormatMap[pixelFormat];
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, vkPixelFormat, &props);

		if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
		    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			return true;
		}
		return false;
	}

	bool VulkanDevice::IsPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice,
	                                            std::shared_ptr<VulkanSurface>& surface) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return false;
		std::cout << deviceProperties.deviceName << std::endl;

		FindQueueFamilies(vkPhysicalDevice, surface);
		bool extensionSupported = CheckDeviceExtensionSupport(vkPhysicalDevice);

		bool swapChainAdequate;
		if (extensionSupported) {
			SwapChainSupportDetails swapChainSupportDetails = surface->QuerySwapchainSupport(vkPhysicalDevice);
			swapChainAdequate = !swapChainSupportDetails.formats.empty() &&
			                    !swapChainSupportDetails.presentModes.empty();
		}

		return m_graphicsFamily.has_value() && m_presentFamily.has_value() && extensionSupported && swapChainAdequate &&
		       deviceFeatures.samplerAnisotropy;
	}

	void VulkanDevice::FindQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily: queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				m_graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface->getSurface(), &presentSupport);

			if (presentSupport) {
				m_presentFamily = i;
			}

			if (m_graphicsFamily.has_value() && m_presentFamily.has_value())
				break;

			i++;
		}
		if (!m_graphicsFamily.has_value() || !m_presentFamily.has_value()) {
			throw std::runtime_error("Failed to get queue indices!");
		}
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
		for (const auto& extension: availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}
	std::optional<uint32_t> VulkanDevice::GetPresentFamilyIndex() const { return m_presentFamily; }

	std::optional<VkQueue> VulkanDevice::GetGraphicsQueue() {

		if (!m_graphicsFamilyQueue.has_value()) {
			VkQueue graphicsQueue;
			vkGetDeviceQueue(m_device.value(), m_graphicsFamily.value(), 0, &graphicsQueue);
			m_graphicsFamilyQueue = std::make_optional(graphicsQueue);
		}
		return m_graphicsFamilyQueue;
	}

	std::optional<VkQueue> VulkanDevice::GetPresentQueue() {
		if (!m_presentFamilyQueue.has_value()) {
			VkQueue presentQueue;
			vkGetDeviceQueue(m_device.value(), m_presentFamily.value(), 0, &presentQueue);
			m_presentFamilyQueue = std::make_optional(presentQueue);
		}
		return m_presentFamilyQueue;
	}
}// namespace Graphics::Vulkan