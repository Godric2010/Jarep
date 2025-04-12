//
// Created by Godri on 3/25/2025.
//

#include "VulkanImage.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanImage::VulkanImage(const VkDevice device, const VkPhysicalDevice physicalDevice, const uint32_t width,
                         const uint32_t height,
                         const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage,
                         const VkMemoryPropertyFlags properties, const VkImageAspectFlags aspect,
                         const VkSampleCountFlagBits sampleCount) {
	m_device = device;
	m_image = VK_NULL_HANDLE;
	m_imageMemory = VK_NULL_HANDLE;
	m_imageView = VK_NULL_HANDLE;

	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.samples = sampleCount;
	createInfo.flags = 0;

	if (vkCreateImage(device, &createInfo, nullptr, &m_image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, m_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, m_image, m_imageMemory, 0);

	VkImageViewCreateInfo viewCreateInfo = {};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = m_image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = format;
	viewCreateInfo.subresourceRange.aspectMask = aspect;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewCreateInfo, nullptr, &m_imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image view!");
	}
}

VulkanImage::~VulkanImage() {
	if (m_imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(m_device, m_imageView, nullptr);
	}
	if (m_image != VK_NULL_HANDLE) {
		vkDestroyImage(m_device, m_image, nullptr);
	}
	if (m_imageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(m_device, m_imageMemory, nullptr);
	}
}

VkImage VulkanImage::getImage() const {
	return m_image;
}

VkImageView VulkanImage::getImageView() const {
	return m_imageView;
}

VkDeviceMemory VulkanImage::getMemory() const {
	return m_imageMemory;
}

uint32_t VulkanImage::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                     VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
