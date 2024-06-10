//
// Created by sebastian on 09.06.24.
//

#include "VulkanRenderPassFramebuffers.hpp"

namespace Graphics::Vulkan {
	VulkanRenderPassFramebuffers::VulkanRenderPassFramebuffers() {
		m_depthFormat = VK_FORMAT_UNDEFINED;
		m_depthImage = nullptr;
		m_depthImageMemory = nullptr;
		m_depthImageView = nullptr;

		m_colorImage = nullptr;
		m_colorImageMemory = nullptr;
		m_colorImageView = nullptr;

		m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	}

	void VulkanRenderPassFramebuffers::CreateRenderPassFramebuffers(std::shared_ptr<VulkanDevice> device,
	                                                                std::shared_ptr<VulkanSurface> surface,
	                                                                VkRenderPass renderPass,
	                                                                VkSampleCountFlagBits multisamplingCount,
	                                                                std::optional<VkFormat> depthFormat) {
		m_device = device;
		VkExtent2D imageExtent = surface->getSwapchain()->getSwapchainImageExtent();
		m_colorFormat = surface->getSwapchain()->getSwapchainImageFormat();
		m_msaaSamples = multisamplingCount;

		createColorResources(imageExtent, m_colorFormat);

		if (depthFormat.has_value()) {
			m_depthFormat = depthFormat.value();
			createDepthResources(imageExtent, m_depthFormat);
		}

		for (const auto& m_swapchainImageView: surface->getSwapchain()->getSwapchainImageViews()) {
			auto framebuffer = std::make_shared<VulkanFramebuffer>(imageExtent);
			framebuffer->CreateFramebuffer(m_device, renderPass, m_swapchainImageView, m_depthImageView,
			                               m_colorImageView);
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

		destroyColorResources();
		destroyDepthResources();
	}

	void VulkanRenderPassFramebuffers::RecreateFramebuffers(VkExtent2D swapchainExtent,
	                                                        std::shared_ptr<VulkanSurface> surface) {
		auto swapchainImageViews = surface->getSwapchain()->getSwapchainImageViews();
		Release();

		createColorResources(swapchainExtent, m_colorFormat);
		if (m_depthFormat != VK_FORMAT_UNDEFINED) {
			createDepthResources(swapchainExtent, m_depthFormat);
		}

		for (int i = 0; i < m_framebuffers.size(); ++i) {
			m_framebuffers[i]->RecreateFramebuffer(swapchainExtent.width, swapchainExtent.height,
			                                       swapchainImageViews[i], m_depthImageView, m_colorImageView);
		}
	}

	void VulkanRenderPassFramebuffers::createDepthResources(VkExtent2D imageExtent, VkFormat depthFormat) {
		VulkanImageBuilder::createImage(m_device, imageExtent.width, imageExtent.height, 1, m_msaaSamples,
		                                depthFormat, VK_IMAGE_TILING_OPTIMAL,
		                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory);
		VulkanImageBuilder::createImageView(m_device, m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT,
		                                    &m_depthImageView, 1);
	}

	void VulkanRenderPassFramebuffers::createColorResources(VkExtent2D imageExtent, VkFormat colorFormat) {
		VulkanImageBuilder::createImage(m_device, imageExtent.width, imageExtent.height, 1, m_msaaSamples,
		                                colorFormat, VK_IMAGE_TILING_OPTIMAL,
		                                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_colorImage, m_colorImageMemory);
		VulkanImageBuilder::createImageView(m_device, m_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT,
		                                    &m_colorImageView, 1);

	}

	void VulkanRenderPassFramebuffers::destroyColorResources() {
		vkDestroyImageView(m_device->getLogicalDevice(), m_colorImageView, nullptr);
		vkDestroyImage(m_device->getLogicalDevice(), m_colorImage, nullptr);
		vkFreeMemory(m_device->getLogicalDevice(), m_colorImageMemory, nullptr);
	}

	void VulkanRenderPassFramebuffers::destroyDepthResources() {
		vkDestroyImageView(m_device->getLogicalDevice(), m_depthImageView, nullptr);
		vkFreeMemory(m_device->getLogicalDevice(), m_depthImageMemory, nullptr);
		vkDestroyImage(m_device->getLogicalDevice(), m_depthImage, nullptr);
	}
}