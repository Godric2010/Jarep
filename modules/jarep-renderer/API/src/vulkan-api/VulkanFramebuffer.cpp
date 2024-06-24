//
// Created by sebastian on 09.06.24.
//

#include "VulkanFramebuffer.hpp"

namespace Graphics::Vulkan {
	VulkanFramebuffer::~VulkanFramebuffer() = default;

	void VulkanFramebuffer::Release() {
		vkDestroyFramebuffer(m_device->getLogicalDevice(), m_framebuffer, nullptr);
	}

	void VulkanFramebuffer::CreateFramebuffer(std::shared_ptr<VulkanDevice> device, VkRenderPass renderPass,
	                                          std::vector<VkImageView> vulkanImageAttachments) {

		m_device = device;
		m_renderPass = renderPass;
		buildFramebuffer(std::move(vulkanImageAttachments));
	}

	void VulkanFramebuffer::RecreateFramebuffer(uint32_t width, uint32_t height, VkImageView swapchainImageView,
	                                            VkImageView depthImageView, VkImageView colorImageView) {

		std::vector<VkImageView> attachments = std::vector<VkImageView>();
		attachments.push_back(colorImageView);
		attachments.push_back(depthImageView);
		attachments.push_back(swapchainImageView);

		m_framebufferExtent.width = width;
		m_framebufferExtent.height = height;
		buildFramebuffer(attachments);
	}

	void VulkanFramebuffer::buildFramebuffer(std::vector<VkImageView> attachments) {

		m_framebufferCreateInfo = {};
		m_framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		m_framebufferCreateInfo.renderPass = m_renderPass;
		m_framebufferCreateInfo.attachmentCount = attachments.size();
		m_framebufferCreateInfo.pAttachments = attachments.data();
		m_framebufferCreateInfo.width = m_framebufferExtent.width;
		m_framebufferCreateInfo.height = m_framebufferExtent.height;
		m_framebufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(m_device->getLogicalDevice(), &m_framebufferCreateInfo, nullptr, &m_framebuffer) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
}