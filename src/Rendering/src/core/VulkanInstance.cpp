//
// Created by sebastian on 1/18/2025.
//

#include "VulkanInstance.hpp"

#include <iostream>
#include <ostream>
using namespace JAREP::Rendering::Core;

VulkanInstance::VulkanInstance(std::vector<const char *> extensions) {
#ifdef _DEBUG
	enable_validation_layers = true;
#else
	enable_validation_layers = false;
#endif

	m_validationLayers = std::vector<const char *>();
	m_extensions = extensions;

	if (enable_validation_layers) {
		if (!checkValidationLayerSupport()) {
			throw std::runtime_error("Validation layers not supported");
		}

		m_validationLayers.push_back("VK_LAYER_KHRONOS_validation");
		m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		createInstance();
		setupDebugMessenger(debugCreateInfo);
	}
	else {
		createInstance();
	}
}

VulkanInstance::~VulkanInstance() = default;

VkInstance VulkanInstance::getInstance() const {
	return m_instance;
}

void VulkanInstance::Destroy() const {
	if (enable_validation_layers) {
		destroyDebugMessenger();
	}
	vkDestroyInstance(m_instance, nullptr);
}

void VulkanInstance::createInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Jarep";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "Jarep";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
	createInfo.ppEnabledExtensionNames = m_extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
	createInfo.ppEnabledLayerNames = m_validationLayers.data();


	vkCreateInstance(&createInfo, nullptr, &m_instance);
}

bool VulkanInstance::checkValidationLayerSupport() const {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName: m_validationLayers) {
		bool layerFound = false;
		for (const auto&layerProperties: availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}

	return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "Validation Layer: " << pUserData << std::endl;
	return VK_FALSE;
}

VkResult VulkanInstance::setupDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
	createInfo.pUserData = nullptr;

	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		return func(m_instance, &createInfo, nullptr, &m_debugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanInstance::destroyDebugMessenger() const {
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		m_instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		func(m_instance, m_debugMessenger, nullptr);
	}
}
