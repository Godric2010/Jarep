//
// Created by sebastian on 09.06.24.
//

#include "VulkanRenderPassFramebuffers.hpp"

#include <utility>

namespace Graphics::Vulkan {
	VulkanRenderPassFramebuffers::VulkanRenderPassFramebuffers(std::shared_ptr<VulkanDevice> device,
	                                                           std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCb) {
		m_device = std::move(device);
		m_createCmdQueueCallback = std::move(createCmdQueueCb);
		m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	}

	void VulkanRenderPassFramebuffers::CreateRenderPassFramebuffers(const std::shared_ptr<VulkanSurface>& surface,
	                                                                VkRenderPass renderPass,
	                                                                std::vector<std::shared_ptr<JarImageBuffer>> imageAttachments) {
//		VkExtent2D imageExtent = surface->getSwapchain()->getSwapchainImageExtent();
//
//		for (const auto& m_swapchainImageView: surface->getSwapchain()->getSwapchainImageViews()) {
//			auto framebuffer = std::make_shared<VulkanFramebuffer_OLD>(imageExtent);
//			std::vector<VkImageView> attachments = std::vector<VkImageView>();
//			for (auto imageAttachment: imageAttachments) {
//				auto vulkanImageBuffer = reinterpret_cast<std::shared_ptr<VulkanImageBuffer>&>(imageAttachment);
//				attachments.push_back(vulkanImageBuffer->GetImageView());
//			}
//			attachments.push_back(m_swapchainImageView);
//
//
//			framebuffer->CreateFramebuffer(m_device, renderPass, attachments);
//			m_framebuffers.push_back(framebuffer);
//		}
	}

	std::shared_ptr<VulkanFramebuffer_OLD> VulkanRenderPassFramebuffers::GetFramebuffer(uint32_t index) {
		return m_framebuffers[index];
	}

	void VulkanRenderPassFramebuffers::Release() {
		for (const auto& framebuffer: m_framebuffers) {
			framebuffer->Release();
		}
	}

	void VulkanRenderPassFramebuffers::RecreateFramebuffers(VkExtent2D swapchainExtent,
	                                                        const std::shared_ptr<VulkanSurface>& surface) {
		throw std::runtime_error("Recreate Framebuffers is not implemented");
//		auto swapchainImageViews = surface->getSwapchain()->getSwapchainImageViews();
//		Release();
//
//
//		for (int i = 0; i < m_framebuffers.size(); ++i) {
//			m_framebuffers[i]->RecreateFramebuffer(swapchainExtent.width, swapchainExtent.height,
//			                                       swapchainImageViews[i], depthImageView,
//			                                       m_colorImageBuffer->GetImageView());
//		}
	}
}