//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANDEVICE_HPP
#define JAREP_VULKANDEVICE_HPP

#include "IRenderAPI.hpp"
#include "VulkanSurface.hpp"
#include <iostream>
#include <set>
#include <string>
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

		void CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface>& surface);

		void CreateLogicalDevice();

		VkDevice GetLogicalDevice() const;

		[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }

		[[nodiscard]] std::optional<uint32_t> GetGraphicsFamilyIndex() const { return m_graphicsFamily; }

		[[nodiscard]] std::optional<uint32_t> GetPresentFamilyIndex() const;

		std::optional<VkQueue> GetGraphicsQueue();

		std::optional<VkQueue> GetPresentQueue();

		private:
		VkPhysicalDevice m_physicalDevice;
		std::optional<VkDevice> m_device;
		std::optional<uint32_t> m_graphicsFamily;
		std::optional<uint32_t> m_presentFamily;
		std::optional<VkQueue> m_graphicsFamilyQueue;
		std::optional<VkQueue> m_presentFamilyQueue;

		const std::vector<const char*> m_deviceExtensions = {
		        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		bool IsPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface);

		void FindQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface);

		bool CheckDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const;
	};
}// namespace Graphics::Vulkan
#endif//JAREP_VULKANDEVICE_HPP
