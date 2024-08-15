//
// Created by sebastian on 15.08.24.
//

#pragma once

#include "../../../include/JarBackend.hpp"
#include <cstdio>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>


class VulkanBackend : public JarBackend {
	public:
	VulkanBackend(const std::vector<const char*>& extensionNames);
	~VulkanBackend();

	private:
	VkInstance m_instance{};
	VkDebugUtilsMessengerEXT m_debugMessenger{};
	std::vector<const char*> m_validationLayers;

	void CreateInstance(const std::vector<const char*>& extensionNames);

	void EnableValidationLayers();

	void CreateDebugCallbackSender();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};
