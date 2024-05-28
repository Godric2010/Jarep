//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalRenderPassImages.hpp"

namespace Graphics::Metal {

	void MetalRenderPassImages::CreateRenderPassImages(uint32_t width, uint32_t height, uint8_t multisamplingCount,
	                                                   MTL::PixelFormat colorFormat,
	                                                   std::optional<MTL::PixelFormat> depthStencilFormat) {
		m_multisamplingCount = multisamplingCount;
		createMSAATexture(width, height, colorFormat, multisamplingCount);
		if (depthStencilFormat.has_value()) {
			m_depthStencilFormat = depthStencilFormat.value();
			createDepthStencilTexture(width, height, depthStencilFormat.value(), m_multisamplingCount);
		}

	}

	void MetalRenderPassImages::RecreateRenderPassImages(uint32_t width, uint32_t height, MTL::PixelFormat msaaFormat) {
		m_msaaTexture->release();
		m_depthStencilTexture->release();
		createMSAATexture(width, height, msaaFormat, m_multisamplingCount);
		if (m_depthStencilTexture != nullptr)
			createDepthStencilTexture(width, height, m_depthStencilFormat, m_multisamplingCount);
	}

	void MetalRenderPassImages::Release() {
		m_msaaTexture->release();
		if (m_depthStencilTexture != nullptr)
			m_depthStencilTexture->release();
	}

	void MetalRenderPassImages::createMSAATexture(uint32_t width, uint32_t height, MTL::PixelFormat format,
	                                              uint8_t multisamplingCount) {
		MTL::TextureDescriptor* msaaDesc = MTL::TextureDescriptor::alloc()->init();
		msaaDesc->setTextureType(MTL::TextureType2DMultisample);
		msaaDesc->setPixelFormat(format);
		msaaDesc->setWidth(width);
		msaaDesc->setHeight(height);
		msaaDesc->setSampleCount(multisamplingCount);
		msaaDesc->setUsage(MTL::TextureUsageRenderTarget);
		m_msaaTexture = m_device->getDevice().value()->newTexture(msaaDesc);

		msaaDesc->release();
	}

	void MetalRenderPassImages::createDepthStencilTexture(uint32_t width, uint32_t height, MTL::PixelFormat format,
	                                                      uint8_t multisamplingCount) {
		MTL::TextureDescriptor* depthStencilDesc = MTL::TextureDescriptor::alloc()->init();
		depthStencilDesc->setTextureType(MTL::TextureType2DMultisample);
		depthStencilDesc->setPixelFormat(format);
		depthStencilDesc->setWidth(width);
		depthStencilDesc->setHeight(height);
		depthStencilDesc->setUsage(MTL::TextureUsageRenderTarget);
		depthStencilDesc->setSampleCount(multisamplingCount);
		m_depthStencilTexture = m_device->getDevice().value()->newTexture(depthStencilDesc);

		depthStencilDesc->release();
	}
}