//
// Created by sebastian on 03.08.24.
//

#pragma once

#include "JarBackend.hpp"
#include <iostream>
#include <vulkan/vulkan.hpp>

class VulkanBackend : public JarBackend {
	public:
	VulkanBackend(const char* const* extensions, size_t count);
	~VulkanBackend() override;

	private:
	VkInstance m_instance{};
	std::vector<const char*> m_extensionNames;
	std::vector<const char*> m_validationLayers;
	VkDebugUtilsMessengerEXT m_debugMessenger{};

	void CreateInstance();

	void EnableValidationLayers();

	void CreateDebugCallbackSender();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	};
};