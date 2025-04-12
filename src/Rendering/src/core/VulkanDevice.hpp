//
// Created by sebastian on 3/22/2025.
//

#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace JAREP::Rendering::Core {
	/**
	 * Encapsulates both the physical and logical Vulkan device. Handles queue family selection and device creation.
	 */
	class VulkanDevice {
		public:
			VulkanDevice(VkInstance instance, VkSurfaceKHR surface);

			~VulkanDevice();

			VulkanDevice(const VulkanDevice&) = delete;

			VulkanDevice& operator=(const VulkanDevice&) = delete;

			VkDevice getDevice() const;

			VkPhysicalDevice getPhysicalDevice() const;

			VkQueue getGraphicsQueue() const;

			VkQueue getPresentQueue() const;

			VkSampleCountFlagBits getMaxSampleCount() const;

			std::optional<uint32_t> getGraphicsQueueFamilyIndex() const;

			uint32_t findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

		private:
			struct QueueFamilyIndices {
				std::optional<uint32_t> graphicsFamily;
				std::optional<uint32_t> presentFamily;

				bool isComplete() const {
					return graphicsFamily.has_value() && presentFamily.has_value();
				}
			};

			void pickPhysicalDevice();

			void createLogicalDevice();

			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

			bool isDeviceSuitable(VkPhysicalDevice device) const;

			bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;

			void getMaxUsableSampleCount();

			VkInstance m_instance;
			VkSurfaceKHR m_surface;
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;
			VkQueue m_graphicsQueue;
			VkQueue m_presentQueue;
			QueueFamilyIndices m_queueFamilies;
			VkSampleCountFlagBits m_maxSampleCount;

			const std::vector<const char *> m_deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};
	};
}
