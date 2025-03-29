//
// Created by Godri on 3/27/2025.
//

#include "VulkanSwapchain.hpp"
#include <stdexcept>
#include <limits>
#include <algorithm>

using namespace JAREP::Rendering::Pipeline;

VulkanSwapchain::VulkanSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                 const SwapchainConfig&config) {
	m_device = device;
	m_physicalDevice = physicalDevice;
	m_surface = surface;
	m_config = config;

	m_swapchain = VK_NULL_HANDLE;
	m_surfaceFormat = {};
	m_presentMode = {};
	m_surfaceCapabilities = {};

	querySupportDetails();
	chooseSurfaceFormat();
	choosePresentMode();
	chooseExtent();
	createSwapchain();
	createImageViews();
}

VulkanSwapchain::~VulkanSwapchain() {
	cleanup();
}

void VulkanSwapchain::querySupportDetails() {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &m_surfaceCapabilities);
}

void VulkanSwapchain::chooseSurfaceFormat() {
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, formats.data());

	for (const auto&available: formats) {
		if (available.format == m_config.preferredFormat && available.colorSpace == m_config.colorSpace) {
			m_surfaceFormat = available;
			return;
		}
	}
	m_surfaceFormat = formats[0]; // fallback
}

void VulkanSwapchain::choosePresentMode() {
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data());
	m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (!m_config.vsync) {
		for (const auto&mode: presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				m_presentMode = mode;
				break;
			}
		}
	}
}

void VulkanSwapchain::chooseExtent() {
	if (m_surfaceCapabilities.currentExtent.width != UINT32_MAX) {
		m_extent = m_surfaceCapabilities.currentExtent;
	}
	else {
		m_extent = {
			std::clamp(m_config.width, m_surfaceCapabilities.minImageExtent.width,
			           m_surfaceCapabilities.maxImageExtent.width),
			std::clamp(m_config.height, m_surfaceCapabilities.minImageExtent.height,
			           m_surfaceCapabilities.maxImageExtent.height),
		};
	}
}

void VulkanSwapchain::createSwapchain() {
	uint32_t imageCount = m_surfaceCapabilities.minImageCount + 1;
	if (m_surfaceCapabilities.maxImageCount > 0 && imageCount > m_surfaceCapabilities.maxImageCount) {
		imageCount = m_surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_surfaceFormat.format;
	createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
	createInfo.imageExtent = m_extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = m_surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = m_swapchain;

	VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &newSwapchain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain!");
	}

	if (m_swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	}
	m_swapchain = newSwapchain;

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
	m_images.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());

	m_format = m_surfaceFormat.format;
}

void VulkanSwapchain::createImageViews() {
	m_imageViews.clear();
	for (const auto&image: m_images) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_format;
		createInfo.components = {};
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView imageView = VK_NULL_HANDLE;
		if (vkCreateImageView(m_device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view!");
		}
		m_imageViews.push_back(imageView);
	}
}

void VulkanSwapchain::cleanup(bool recreate) {
	for (auto view: m_imageViews) {
		vkDestroyImageView(m_device, view, nullptr);
	}
	m_imageViews.clear();

	if (recreate) {
		return;
	}

	if (m_swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	}
}

void VulkanSwapchain::recreate(uint32_t width, uint32_t height) {
	cleanup(true);
	m_config.width = width;
	m_config.height = height;

	querySupportDetails();
	chooseExtent();
	createSwapchain();
	createImageViews();
}

VkSwapchainKHR VulkanSwapchain::get() const {
	return m_swapchain;
}

const std::vector<VkImageView>& VulkanSwapchain::getImageViews() const {
	return m_imageViews;
}

VkFormat VulkanSwapchain::getFormat() const {
	return m_format;
}

VkExtent2D VulkanSwapchain::getExtent() const {
	return m_extent;
}

uint32_t VulkanSwapchain::getImageCount() const {
	return m_images.size();
}

