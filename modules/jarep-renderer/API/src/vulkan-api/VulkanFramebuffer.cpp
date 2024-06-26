//
// Created by sebastian on 24.06.24.
//

#include "VulkanFramebuffer.hpp"
#include "VulkanDevice.hpp"

namespace Graphics::Vulkan {

	VulkanFramebuffer::VulkanFramebuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanImageBuffer> targetBuffer,
	                                     VkFramebuffer framebuffer, VkExtent2D framebufferExtent) {
		m_device = device;
		m_targetImageBuffer = targetBuffer;
		m_framebuffer = framebuffer;
		m_framebufferExtent = framebufferExtent;
	}

	void VulkanFramebuffer::Release() {

		vkDeviceWaitIdle(m_device->GetLogicalDevice());
		vkDestroyFramebuffer(m_device->GetLogicalDevice(), m_framebuffer, nullptr);
		m_targetImageBuffer->Release();
	}

	VkImage VulkanFramebuffer::GetSrcImage() { return m_targetImageBuffer->GetImage(); }
}// namespace Graphics::Vulkan