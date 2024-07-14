//
// Created by Sebastian Borsch on 13.07.24.
//

#include "MetalFramebuffer.hpp"

namespace Graphics::Metal{

	MetalFramebuffer::MetalFramebuffer(std::shared_ptr<MetalImageBuffer> targetImageBuffer) {
		m_renderTargetBuffer = targetImageBuffer;
	}

	void MetalFramebuffer::Release() {
		m_renderTargetBuffer->Release();
	}
}