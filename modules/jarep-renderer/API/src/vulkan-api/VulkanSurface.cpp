//
// Created by sebastian on 09.06.24.
//

#include "VulkanSurface.hpp"

namespace Graphics::Vulkan {
	VulkanSurface::VulkanSurface(VkSurfaceKHR surface, VkExtent2D surfaceExtend) {
		m_surface = surface;
		m_surfaceExtent = surfaceExtend;
	}

	VulkanSurface::~VulkanSurface() = default;

	void VulkanSurface::RecreateSurface(uint32_t width, uint32_t height) {

		vkDeviceWaitIdle(m_device->getLogicalDevice());

		m_surfaceExtent.width = width;
		m_surfaceExtent.height = height;

		auto swapchainSupport = QuerySwapchainSupport(m_device->getPhysicalDevice());

		m_swapchain->RecreateSwapchain(width, height, swapchainSupport);
	}

	void VulkanSurface::ReleaseSwapchain() {
		m_swapchain->Release();
	}

	uint32_t VulkanSurface::GetSwapchainImageAmount() {
		uint32_t maxSwapchainImages = m_swapchain->getMaxSwapchainImageCount();
		return maxSwapchainImages;
	}

	JarExtent VulkanSurface::GetSurfaceExtent() {
		JarExtent extent{};
		extent.Width = m_surfaceExtent.width;
		extent.Height = m_surfaceExtent.height;
		return extent;
	}

	void VulkanSurface::FinalizeSurface(std::shared_ptr<VulkanDevice> device) {
		m_device = device;

		auto swapchainSupport = QuerySwapchainSupport(m_device->getPhysicalDevice());

		m_swapchain = std::make_unique<VulkanSwapchain>(m_device, m_surface);
		m_swapchain->Initialize(m_surfaceExtent, swapchainSupport);
	}


	SwapChainSupportDetails VulkanSurface::QuerySwapchainSupport(VkPhysicalDevice physicalDevice) const {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &details.capabilities);


		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount,
			                                          details.presentModes.data());
		}

		return details;
	}
}