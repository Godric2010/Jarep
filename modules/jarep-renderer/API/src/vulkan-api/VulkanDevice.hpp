//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANDEVICE_HPP
#define JAREP_VULKANDEVICE_HPP

#include "IRenderAPI.hpp"
#include "VulkanSurface.hpp"
#include <iostream>
#include <string>
#include <set>
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanSurface;
	class VulkanDevice final : public JarDevice {
		public:
			VulkanDevice();

			~VulkanDevice() override;

			void Release() override;

			uint32_t GetMaxUsableSampleCount() override;

			bool IsFormatSupported(PixelFormat pixelFormat) override;

			void CreatePhysicalDevice(VkInstance instance, std::shared_ptr <VulkanSurface>& surface);

			void CreateLogicalDevice();

			[[nodiscard]] VkDevice getLogicalDevice() const { return m_device; }

			[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }

			[[nodiscard]] std::optional <uint32_t> getGraphicsFamilyIndex() const { return m_graphicsFamily; }

			[[nodiscard]] std::optional <uint32_t> getPresentFamilyIndex() const { return m_presentFamily; }

		private:
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;
			std::optional <uint32_t> m_graphicsFamily;
			std::optional <uint32_t> m_presentFamily;

			const std::vector<const char*> deviceExtensions = {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			bool isPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr <VulkanSurface>& surface);

			void findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr <VulkanSurface>& surface);

			bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const;


	};
}
#endif //JAREP_VULKANDEVICE_HPP
