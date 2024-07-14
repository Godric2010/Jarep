//
// Created by Sebastian Borsch on 19.06.24.
//

#include "MetalFramebufferBuilder.hpp"

namespace Graphics::Metal {

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
		m_renderPass = std::make_optional(reinterpret_cast<std::shared_ptr<MetalRenderPass>&>(renderPass));
		return this;
	}

	MetalFramebufferBuilder*
	MetalFramebufferBuilder::SetImageBuffers(std::vector<std::shared_ptr<JarImageBuffer>> imageBuffers) {
		for (int i = 0; i < imageBuffers.size(); ++i) {
			auto metalImageBuffer = reinterpret_cast<std::shared_ptr<MetalImageBuffer>&>(imageBuffers[i]);
			m_imageBuffers.push_back(metalImageBuffer);

			if (i == imageBuffers.size() - 1) {
				m_targetImageBuffer = std::make_optional(metalImageBuffer);
			}
		}
		return this;
	}

	std::shared_ptr<JarFramebuffer> MetalFramebufferBuilder::Build(std::shared_ptr<JarDevice> device) {
		if (m_imageBuffers.empty() || !m_width.has_value() || !m_height.has_value() || !m_pixelFormat.has_value() ||
		    !m_renderPass.has_value())
			throw std::runtime_error(
					"MetalFramebufferBuilder: Build() called without setting all required parameters");

		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		m_renderPass.value()->SetRenderPassImageBuffers(m_imageBuffers[0], m_imageBuffers[1], m_imageBuffers[2]);

		return std::make_shared<MetalFramebuffer>(m_targetImageBuffer.value());
	}
}