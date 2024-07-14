//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalRenderPass.hpp"

namespace Graphics::Metal {

	MetalRenderPass::~MetalRenderPass() = default;

	void MetalRenderPass::Release() {
		renderPassImages->Release();
	}

	void MetalRenderPass::SetRenderPassImageBuffers(std::shared_ptr<MetalImageBuffer> msaaImageBuffer,
	                                                std::shared_ptr<MetalImageBuffer> depthStencilImageBuffer,
	                                                std::shared_ptr<MetalImageBuffer> targetImageBuffer) {
		renderPassDesc->colorAttachments()->object(0)->setTexture(msaaImageBuffer->GetTexture());
		renderPassDesc->colorAttachments()->object(0)->setResolveTexture(targetImageBuffer->GetTexture());
		renderPassDesc->depthAttachment()->setTexture(depthStencilImageBuffer->GetTexture());
	}

	void MetalRenderPass::UpdateRenderPassDescriptor(std::shared_ptr<MetalSurface> metalSurface) {
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderPassImages->getMSAATexture());
		renderPassDesc->colorAttachments()->object(0)->setResolveTexture(metalSurface->getDrawable()->texture());
		renderPassDesc->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
	}
}