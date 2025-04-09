//
// Created by Godri on 4/9/2025.
//

#include "VulkanDepthBuffer.hpp"

#include <stdexcept>
#include <vector>

using namespace JAREP::Rendering::Pipeline;

VulkanDepthBuffer::VulkanDepthBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent,
                                     bool enableStencil) {
	m_device = device;

	m_format = VK_FORMAT_UNDEFINED;
	m_depthImage = VK_NULL_HANDLE;
	m_depthImageView = VK_NULL_HANDLE;
	m_depthImageMemory = VK_NULL_HANDLE;

	findDepthFormat(physicalDevice, enableStencil);
	createImage(physicalDevice, extent);
	createImageView(enableStencil);
}

VulkanDepthBuffer::~VulkanDepthBuffer() {
	if (m_depthImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(m_device, m_depthImageView, nullptr);
	}

	if (m_depthImage != VK_NULL_HANDLE) {
		vkDestroyImage(m_device, m_depthImage, nullptr);
	}

	if (m_depthImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(m_device, m_depthImageMemory, nullptr);
	}
}

VkImage VulkanDepthBuffer::getDepthImage() const {
	return m_depthImage;
}


VkFormat VulkanDepthBuffer::getFormat() const {
	return m_format;
}

VkImageView VulkanDepthBuffer::getImageView() const {
	return m_depthImageView;
}

void VulkanDepthBuffer::findDepthFormat(VkPhysicalDevice physicalDevice, bool enableStencil) {
	std::vector<VkFormat> formats;
	if (enableStencil) {
		formats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	}
	else {
		formats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM};
	}

	for (VkFormat format: formats) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
		if ((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
		    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			m_format = format;
			return;
		}
		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
		    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			m_format = format;
			return;
		}
	}

	throw std::runtime_error("Could not find a valid depth format!");
}


void VulkanDepthBuffer::createImage(VkPhysicalDevice physicalDevice, VkExtent2D extent) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = extent.width;
	imageInfo.extent.height = extent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = m_format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(m_device, &imageInfo, nullptr, &m_depthImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to depth create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, m_depthImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	bool memoryTypeFound = false;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) && (
			    memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			allocInfo.memoryTypeIndex = i;
			memoryTypeFound = true;
		}
	}
	if (!memoryTypeFound || vkAllocateMemory(m_device, &allocInfo, nullptr, &m_depthImageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate memory for depth image!");
	}

	vkBindImageMemory(m_device, m_depthImage, m_depthImageMemory, 0);
}

void VulkanDepthBuffer::createImageView(bool enableStencil) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_depthImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = m_format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (enableStencil) {
		viewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_depthImageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create depth image view!");
	}
}
