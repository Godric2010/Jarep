//
// Created by sebastian on 09.06.24.
//

#include "VulkanRenderPass.hpp"

namespace Graphics::Vulkan {
	VulkanRenderPass::~VulkanRenderPass() = default;

	void VulkanRenderPass::Release() {
		m_framebuffers->Release();
		vkDestroyRenderPass(m_device->getLogicalDevice(), m_renderPass, nullptr);
	}

	void VulkanRenderPass::RecreateRenderPassFramebuffers(uint32_t width, uint32_t height,
	                                                      std::shared_ptr<JarSurface> surface) {
		auto vulkanSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);
		m_framebuffers->RecreateFramebuffers({width, height}, vulkanSurface);
	}

	std::shared_ptr<VulkanFramebuffer> VulkanRenderPass::AcquireNextFramebuffer(uint32_t frameIndex) {
		return m_framebuffers->GetFramebuffer(frameIndex);
	};
}