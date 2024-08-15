//
// Created by sebastian on 15.08.24.
//

#include "VulkanBackend.hpp"

VulkanBackend::VulkanBackend(const std::vector<const char*>& extensionNames) {

	auto extensions = extensionNames;

#if defined(VALIDATION_LAYERS_ENABLED)
	EnableValidationLayers();
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	CreateInstance(extensions);
	CreateDebugCallbackSender();
#else
	createInstance(extensions);
#endif
}

VulkanBackend::~VulkanBackend() { vkDestroyInstance(m_instance, nullptr); }


void VulkanBackend::CreateInstance(const std::vector<const char*>& extensionNames) {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "JAREP";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "JAREP";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
	createInfo.ppEnabledLayerNames = m_validationLayers.data();
	createInfo.pApplicationInfo = &appInfo;

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) { throw std::runtime_error("failed to create instance!"); }
}

void VulkanBackend::EnableValidationLayers() {

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

	for (const char* layerName: m_validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties: availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) { throw std::runtime_error("Layer not found"); }
	}
}

void VulkanBackend::CreateDebugCallbackSender() {
	PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerExt = nullptr;
	createDebugUtilsMessengerExt =
	        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));

	if (!createDebugUtilsMessengerExt) throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT");


	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = DebugCallback;
	if (createDebugUtilsMessengerExt(m_instance, &debugCreateInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}
