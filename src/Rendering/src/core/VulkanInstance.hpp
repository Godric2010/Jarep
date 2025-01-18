//
// Created by sebastian on 1/18/2025.
//

#pragma once

#include <vulkan\vulkan.hpp>

namespace JAREP::Rendering::Core {
    class VulkanInstance {
    public:
        VulkanInstance();

        ~VulkanInstance();

        VkInstance getInstance() const;

        void Destroy();

    private:
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;

        void createInstance();

        void setupDebugMessenger();
    };
}
