//
// Created by Sebastian Borsch on 12.06.24.
//

#include "MetalRenderTargetBuilder.hpp"

namespace Graphics::Metal {

	MetalRenderTargetBuilder::~MetalRenderTargetBuilder() {
	}

	MetalRenderTargetBuilder* MetalRenderTargetBuilder::SetRenderTargetType(RenderTargetType type) {
		m_renderTargetType = std::make_optional(type);
		return this;
	}

	MetalRenderTargetBuilder* MetalRenderTargetBuilder::SetResolution(uint32_t width, uint32_t height) {
		m_width = std::make_optional(width);
		m_height = std::make_optional(height);
		return this;
	}

	MetalRenderTargetBuilder* MetalRenderTargetBuilder::SetImageFormat(PixelFormat format) {
		m_pixelFormat = std::make_optional(format);
		return this;
	}

	std::shared_ptr<JarRenderTarget> MetalRenderTargetBuilder::Build() {
		if (!m_renderTargetType.has_value() || !m_width.has_value() || !m_height.has_value() ||
		    !m_pixelFormat.has_value())
			throw std::runtime_error(
					"MetalRenderTargetBuilder: Build() called without setting all required parameters");
		return std::make_shared<MetalRenderTarget>(m_renderTargetType.value(), m_width.value(), m_height.value(),
		                                           m_pixelFormat.value());
	}
}