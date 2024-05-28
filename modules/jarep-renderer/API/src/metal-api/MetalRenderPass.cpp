//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalRenderPass.hpp"

namespace Graphics::Metal {

	MetalRenderPass::~MetalRenderPass() = default;

	void MetalRenderPass::Release() {
		renderPassImages->Release();
	}

	void MetalRenderPass::RecreateRenderPassFramebuffers(uint32_t width, uint32_t height,
	                                                     std::shared_ptr<JarSurface> surface) {
		auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);
		renderPassImages->RecreateRenderPassImages(width, height, metalSurface->getDrawablePixelFormat());
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderPassImages->getMSAATexture());
		renderPassDesc->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
	}

	void MetalRenderPass::UpdateRenderPassDescriptor(std::shared_ptr<MetalSurface> metalSurface) {
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderPassImages->getMSAATexture());
		renderPassDesc->colorAttachments()->object(0)->setResolveTexture(metalSurface->getDrawable()->texture());
		renderPassDesc->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
	}
}