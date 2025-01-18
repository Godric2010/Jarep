//
// Created by sebastian on 1/18/2025.
//

#include "VulkanInstance.hpp"
using namespace JAREP::Rendering::Core;

VulkanInstance::VulkanInstance() {
    createInstance();
}

VulkanInstance::~VulkanInstance() = default;

VkInstance VulkanInstance::getInstance() const {
    return m_instance;
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
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.enabledLayerCount =0;
    createInfo.ppEnabledLayerNames = nullptr;

    vkCreateInstance(&createInfo, nullptr, &m_instance);
}




