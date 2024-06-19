//
// Created by Sebastian Borsch on 19.06.24.
//

#include "MetalFramebufferBuilder.hpp"

namespace Graphics::Metal{

	MetalFramebufferBuilder* MetalFramebufferBuilder::SetFramebufferExtent(uint32_t width, uint32_t height) {
		m_width = std::make_optional(width);
		m_height = std::make_optional(height);
		return this;
	}

	MetalFramebufferBuilder* MetalFramebufferBuilder::SetImageFormat(PixelFormat format) {
		m_pixelFormat = std::make_optional(format);
		return this;
	}

	MetalFramebufferBuilder* MetalFramebufferBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = std::make_optional(renderPass);
		return this;
	}

	std::shared_ptr<JarFramebuffer> MetalFramebufferBuilder::Build(std::shared_ptr<JarDevice> device) {
		if (!m_width.has_value() || !m_height.has_value() || !m_pixelFormat.has_value() || !m_renderPass.has_value())
			throw std::runtime_error(
					"MetalFramebufferBuilder: Build() called without setting all required parameters");
		return nullptr;
	}
}