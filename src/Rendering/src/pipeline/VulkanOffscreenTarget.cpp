//
// Created by Godri on 3/30/2025.
//

#include "VulkanOffscreenTarget.hpp"
#include <stdexcept>
#include "../core/VulkanCore.hpp"
#include "../core/VulkanImage.hpp"

using namespace JAREP::Rendering::Pipeline;

VulkanOffscreenTarget::VulkanOffscreenTarget(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D resolution,
                                             VkFormat format, VkRenderPass renderPass,
                                             std::optional<VkImageView> depthImageView,
                                             VkSampleCountFlagBits sampleCountFlagBits) {
	m_device = device;
	m_physicalDevice = physicalDevice;
	m_extent = resolution;
	m_format = format;

	m_framebuffer = VK_NULL_HANDLE;

	m_multisamplingEnabled = sampleCountFlagBits != VK_SAMPLE_COUNT_1_BIT;

	createImage(sampleCountFlagBits);
	if (m_multisamplingEnabled) {
		createResolveImage();
	}
	createFramebuffer(renderPass, depthImageView);
}

VulkanOffscreenTarget::~VulkanOffscreenTarget() {
	if (m_framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
	}
	m_colorImage.reset();
	m_resolveImage.reset();
}

VkExtent2D VulkanOffscreenTarget::getExtent() const {
	return m_extent;
}

VkFramebuffer VulkanOffscreenTarget::getFramebuffer() const {
	return m_framebuffer;
}

VkImageView VulkanOffscreenTarget::getImageView() const {
	return m_multisamplingEnabled ? m_resolveImage->getImageView() : m_colorImage->getImageView();
}

VkImage VulkanOffscreenTarget::getImage() const {
	return m_multisamplingEnabled ? m_resolveImage->getImage() : m_colorImage->getImage();
}

void VulkanOffscreenTarget::createImage(VkSampleCountFlagBits sampleCountFlagBits) {
	m_colorImage = std::make_unique<Core::VulkanImage>(m_device,
	                                                   m_physicalDevice,
	                                                   m_extent.width,
	                                                   m_extent.height,
	                                                   m_format,
	                                                   VK_IMAGE_TILING_OPTIMAL,
	                                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
	                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	                                                   VK_IMAGE_ASPECT_COLOR_BIT,
	                                                   sampleCountFlagBits
	);
}

void VulkanOffscreenTarget::createResolveImage() {
	m_resolveImage = std::make_unique<Core::VulkanImage>(m_device,
	                                                     m_physicalDevice,
	                                                     m_extent.width,
	                                                     m_extent.height,
	                                                     m_format,
	                                                     VK_IMAGE_TILING_OPTIMAL,
	                                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
	                                                     VK_IMAGE_USAGE_SAMPLED_BIT,
	                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	                                                     VK_IMAGE_ASPECT_COLOR_BIT);
}

void VulkanOffscreenTarget::createFramebuffer(VkRenderPass renderPass, std::optional<VkImageView> depthImageView) {
	std::vector<VkImageView> attachments = {m_colorImage->getImageView()};
	if (m_multisamplingEnabled) {
		attachments.push_back(m_resolveImage->getImageView());
	}
	if (depthImageView.has_value()) {
		attachments.push_back(depthImageView.value());
	}


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
