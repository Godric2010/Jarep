//
// Created by Godri on 3/28/2025.
//

#include "VulkanFramebuffer.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Pipeline;

VulkanFramebuffer::VulkanFramebuffer(VkDevice device, VkRenderPass renderPass, VkExtent2D extent,
                                     const std::vector<VkImageView>&attachments) {
	m_device = device;
	m_renderPass = renderPass;
	m_extent = extent;
	m_attachments = attachments;
	m_framebuffer = VK_NULL_HANDLE;

	createFramebuffer();
}

VulkanFramebuffer::~VulkanFramebuffer() {
	if (m_framebuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
	}
}

VkFramebuffer VulkanFramebuffer::get() const {
	return m_framebuffer;
}

void VulkanFramebuffer::createFramebuffer() {
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = m_renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
	framebufferInfo.pAttachments = m_attachments.data();
	framebufferInfo.width = m_extent.width;
	framebufferInfo.height = m_extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer");
	}
}
