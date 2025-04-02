//
// Created by Godri on 3/30/2025.
//

#include "VulkanOffscreenTarget.hpp"
#include <stdexcept>
#include <array>

using namespace JAREP::Rendering::Pipeline;

VulkanOffscreenTarget::VulkanOffscreenTarget(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D resolution,
                                             VkFormat format, VkRenderPass renderPass) {
	m_device = device;
	m_physicalDevice = physicalDevice;
	m_extent = resolution;
	m_format = format;

	m_image = VK_NULL_HANDLE;
	m_memory = VK_NULL_HANDLE;
	m_imageView = VK_NULL_HANDLE;
	m_framebuffer = VK_NULL_HANDLE;

	createImage();
	createImageView();
	createFramebuffer(renderPass);
}

VulkanOffscreenTarget::~VulkanOffscreenTarget() {
	if (m_framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
	}
	if (m_imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(m_device, m_imageView, nullptr);
	}
	if (m_image != VK_NULL_HANDLE) {
		vkDestroyImage(m_device, m_image, nullptr);
	}
	if (m_memory != VK_NULL_HANDLE) {
		vkFreeMemory(m_device, m_memory, nullptr);
	}
}

VkExtent2D VulkanOffscreenTarget::getExtent() const {
	return m_extent;
}

VkFramebuffer VulkanOffscreenTarget::getFramebuffer() const {
	return m_framebuffer;
}

VkImageView VulkanOffscreenTarget::getImageView() const {
	return m_imageView;
}

void VulkanOffscreenTarget::createImage() {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = m_extent.width;
	imageInfo.extent.height = m_extent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = m_format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_device, &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, m_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

	bool memoryTypeFound = false;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) && (
			    memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			allocInfo.memoryTypeIndex = i;
			memoryTypeFound = true;
			break;
		}
	}
	if (!memoryTypeFound || vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate offscreen image memory!");
	}

	vkBindImageMemory(m_device, m_image, m_memory, 0);
}

void VulkanOffscreenTarget::createImageView() {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = m_format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create offscreen image view!");
	}
}

void VulkanOffscreenTarget::createFramebuffer(VkRenderPass renderPass) {
	std::array<VkImageView, 1> attachments = {m_imageView};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = m_extent.width;
	framebufferInfo.height = m_extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create offscreen framebuffer!");
	}
}
