//
// Created by sebastian on 18.06.24.
//

#include "VulkanFramebufferBuilder.hpp"

namespace Graphics::Vulkan {

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = std::make_optional(reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass));
		return this;
	}

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetFramebufferExtent(uint32_t width, uint32_t height) {
		m_framebufferExtent = std::make_optional<VkExtent2D>({width, height});
		return this;
	}

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetImageFormat(PixelFormat format) {
		m_imageFormat = std::optional(pixelFormatMap[format]);
		return this;
	}

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetImageBuffers(std::vector<std::shared_ptr<JarImageBuffer>> imageBuffers) {
		for (int i = 0; i < imageBuffers.size(); ++i) {
			auto vulkanImageBuffer = reinterpret_cast<std::shared_ptr<VulkanImageBuffer>&>(imageBuffers[i]);
			m_imageViews.push_back(vulkanImageBuffer->GetImageView());

			if (i == imageBuffers.size() - 1) {
				m_targetImageBuffer = std::make_optional(vulkanImageBuffer);
			}
		}
		return this;
	}

	std::shared_ptr<JarFramebuffer> VulkanFramebufferBuilder::Build(std::shared_ptr<JarDevice> device) {
		if (!m_targetImageBuffer.has_value() || !m_imageFormat.has_value() || !m_renderPass.has_value() || !m_framebufferExtent.has_value())
			throw std::runtime_error("Framebuffer builder is missing settings on build!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_renderPass.value()->GetRenderPass();
		framebufferCreateInfo.attachmentCount = m_imageViews.size();
		framebufferCreateInfo.pAttachments = m_imageViews.data();
		framebufferCreateInfo.width = m_framebufferExtent.value().width;
		framebufferCreateInfo.height = m_framebufferExtent.value().height;
		framebufferCreateInfo.layers = 1;

		VkFramebuffer framebuffer;
		if (vkCreateFramebuffer(vulkanDevice->getLogicalDevice(), &framebufferCreateInfo, nullptr, &framebuffer) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}

		return std::make_shared<VulkanFramebuffer>(vulkanDevice, m_targetImageBuffer.value(), framebuffer, m_framebufferExtent.value());
	}


}// namespace Graphics::Vulkan