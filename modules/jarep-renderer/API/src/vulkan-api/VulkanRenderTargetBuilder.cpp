//
// Created by sebastian on 14.06.24.
//

#include "VulkanRenderTargetBuilder.hpp"

namespace Graphics::Vulkan {
	VulkanRenderTargetBuilder* VulkanRenderTargetBuilder::SetResolution(uint32_t width, uint32_t height) {
		m_width = std::make_optional(width);
		m_height = std::make_optional(height);
		return this;
	}

	VulkanRenderTargetBuilder*
	VulkanRenderTargetBuilder::SetRenderTargetType(Graphics::RenderTargetType renderTargetType) {
		m_type = std::make_optional(renderTargetType);
		return this;
	}

	VulkanRenderTargetBuilder* VulkanRenderTargetBuilder::SetImageFormat(Graphics::PixelFormat pixelFormat) {
		m_format = std::make_optional(pixelFormatMap[pixelFormat]);
		return this;
	}

	std::shared_ptr<JarRenderTarget> VulkanRenderTargetBuilder::Build() {
		if (!m_width.has_value() || !m_height.has_value() || !m_format.has_value() || !m_type.has_value())
			throw std::runtime_error("VulkanRenderTarget has some uninitialized fields!");


		std::shared_ptr<VulkanRenderTarget> vulkanRenderTarget = std::make_shared<VulkanRenderTarget>(m_width.value(),
		                                                                                             m_height.value(),
		                                                                                             m_format.value(),
		                                                                                             m_type.value());
		return vulkanRenderTarget;
	}
}