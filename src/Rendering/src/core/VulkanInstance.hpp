//
// Created by sebastian on 1/18/2025.
//

#pragma once

#include <vulkan\vulkan.hpp>

namespace JAREP::Rendering::Core {
	/**
	 * Initializes the Vulkan API and creates a VkInstance. Responsible for managing Vulkan extensions and validation layers.
	 */
	class VulkanInstance {
		public:
			VulkanInstance(std::vector<const char *> extensions);

			~VulkanInstance();

			VulkanInstance(const VulkanInstance&) = delete;

			VulkanInstance& operator=(const VulkanInstance&) = delete;

			VkInstance getInstance() const;

			void Destroy() const;

		private:
			VkInstance m_instance;
			VkDebugUtilsMessengerEXT m_debugMessenger;
			std::vector<const char *> m_validationLayers;
			std::vector<const char *> m_extensions;
			bool enable_validation_layers;

			void createInstance();

			bool checkValidationLayerSupport() const;

			std::vector<const char *> getRequiredExtensions();

			VkResult setupDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT);

			void destroyDebugMessenger() const;
	};
}
