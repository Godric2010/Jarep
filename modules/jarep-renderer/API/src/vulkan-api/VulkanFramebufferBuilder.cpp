//
// Created by sebastian on 18.06.24.
//

#include "VulkanFramebufferBuilder.hpp"

namespace Graphics::Vulkan {

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);
		return this;
	}

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetFramebufferExtent(uint32_t width, uint32_t height) {
		m_framebufferExtent = {width, height};
		return this;
	}

	VulkanFramebufferBuilder* VulkanFramebufferBuilder::SetImageFormat(PixelFormat format) {
		m_imageFormat = pixelFormatMap[format];
		return this;
	}

	std::shared_ptr<JarFramebuffer> VulkanFramebufferBuilder::Build(std::shared_ptr<JarDevice> device) {
		return nullptr;
	}


}