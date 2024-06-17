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
		m_colorImageBuffer = nullptr;
		m_depthImageBuffer = std::nullopt;
	}

	void VulkanRenderPassFramebuffers::CreateRenderPassFramebuffers(const std::shared_ptr<VulkanSurface>& surface,
	                                                                VkRenderPass renderPass,
	                                                                VkSampleCountFlagBits multisamplingCount,
	                                                                std::optional<VkFormat> depthFormat) {
		VkExtent2D imageExtent = surface->getSwapchain()->getSwapchainImageExtent();
		m_colorFormat = surface->getSwapchain()->getSwapchainImageFormat();
		m_msaaSamples = multisamplingCount;

		createColorResources(imageExtent, m_colorFormat);

		if (depthFormat.has_value()) {
			createDepthResources(imageExtent, depthFormat.value());
		}

		for (const auto& m_swapchainImageView: surface->getSwapchain()->getSwapchainImageViews()) {
			auto framebuffer = std::make_shared<VulkanFramebuffer>(imageExtent);
			framebuffer->CreateFramebuffer(m_device, renderPass, m_swapchainImageView,
			                               m_depthImageBuffer.value()->GetImageView(),
			                               m_colorImageBuffer->GetImageView());
			m_framebuffers.push_back(framebuffer);
		}
	}

	std::shared_ptr<VulkanFramebuffer> VulkanRenderPassFramebuffers::GetFramebuffer(uint32_t index) {
		return m_framebuffers[index];
	}

	void VulkanRenderPassFramebuffers::Release() {
		for (const auto& framebuffer: m_framebuffers) {
			framebuffer->Release();
		}
		m_colorImageBuffer->Release();
		if (m_depthImageBuffer.has_value()) {
			m_depthImageBuffer.value()->Release();
		}
	}

	void VulkanRenderPassFramebuffers::RecreateFramebuffers(VkExtent2D swapchainExtent,
	                                                        const std::shared_ptr<VulkanSurface>& surface) {
		auto swapchainImageViews = surface->getSwapchain()->getSwapchainImageViews();
		Release();

		createColorResources(swapchainExtent, m_colorFormat);
		VkImageView depthImageView = nullptr;
		if (m_depthImageBuffer.has_value()) {
			createDepthResources(swapchainExtent, m_depthImageBuffer.value()->GetFormat());
			depthImageView = m_depthImageBuffer.value()->GetImageView();
		}
		for (int i = 0; i < m_framebuffers.size(); ++i) {
			m_framebuffers[i]->RecreateFramebuffer(swapchainExtent.width, swapchainExtent.height,
			                                       swapchainImageViews[i], depthImageView,
			                                       m_colorImageBuffer->GetImageView());
		}
	}

	void VulkanRenderPassFramebuffers::createDepthResources(VkExtent2D imageExtent, VkFormat depthFormat) {
		m_depthImageBuffer = std::optional(
				std::make_unique<VulkanImageBuffer>(m_device, m_createCmdQueueCallback, imageExtent,
				                                    depthFormat, 1, m_msaaSamples, VK_IMAGE_TILING_OPTIMAL,
				                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				                                    VK_IMAGE_ASPECT_DEPTH_BIT));
	}

	void VulkanRenderPassFramebuffers::createColorResources(VkExtent2D imageExtent, VkFormat colorFormat) {
		m_colorImageBuffer = std::make_unique<VulkanImageBuffer>(m_device, m_createCmdQueueCallback, imageExtent,
		                                                         colorFormat, 1, m_msaaSamples, VK_IMAGE_TILING_OPTIMAL,
		                                                         VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
		                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                                                         VK_IMAGE_ASPECT_COLOR_BIT);

	}
}