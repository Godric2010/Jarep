//
// Created by sebastian on 3/22/2025.
//

#include "VulkanDevice.hpp"

#include <set>
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface) {
	m_instance = instance;
	m_surface = surface;
	m_physicalDevice = VK_NULL_HANDLE;
	m_device = VK_NULL_HANDLE;
	m_graphicsQueue = VK_NULL_HANDLE;
	m_presentQueue = VK_NULL_HANDLE;
	pickPhysicalDevice();
	createLogicalDevice();
}

VulkanDevice::~VulkanDevice() {
	if (m_device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(m_device);
		vkDestroyDevice(m_device, nullptr);
	}
}

VkDevice VulkanDevice::getDevice() const {
	return m_device;
}

VkPhysicalDevice VulkanDevice::getPhysicalDevice() const {
	return m_physicalDevice;
}

VkQueue VulkanDevice::getGraphicsQueue() const {
	return m_graphicsQueue;
}

VkQueue VulkanDevice::getPresentQueue() const {
	return m_presentQueue;
}


void VulkanDevice::pickPhysicalDevice() {
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support");
	}
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

	for (const auto&device: physicalDevices) {
		if (isDeviceSuitable(device)) {
			m_physicalDevice = device;
			break;
		}
	}
	if (m_physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("No suitable GPU found");
	}
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) const {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(device);
	bool extensionSupported = checkDeviceExtensionSupport(device);

	return queueFamilyIndices.isComplete() && extensionSupported;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	std::set<std::string> required(m_deviceExtensions.begin(), m_deviceExtensions.end());
	for (const auto&extension: extensions) {
		required.erase(extension.extensionName);
	}
	return required.empty();
}

void VulkanDevice::createLogicalDevice() {
	m_queueFamilies = {};
	m_queueFamilies = findQueueFamilies(m_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		m_queueFamilies.graphicsFamily.value(),
		m_queueFamilies.presentFamily.value()
	};

	float priority = 1.0f;
	for (uint32_t queueFamily: uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &priority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device)) {
		throw std::runtime_error("failed to create logical device");
	}
	vkGetDeviceQueue(m_device, m_queueFamilies.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_queueFamilies.presentFamily.value(), 0, &m_presentQueue);
}

uint32_t VulkanDevice::findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type");
}

VulkanDevice::QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const {
	QueueFamilyIndices queueFamilyIndices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto&queueFamily: queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilyIndices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (presentSupport) {
			queueFamilyIndices.presentFamily = i;
		}
		if (queueFamilyIndices.isComplete()) {
			break;
		}
		++i;
	}
	return queueFamilyIndices;
}
