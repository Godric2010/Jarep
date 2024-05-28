//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalRenderPassBuilder.hpp"

namespace Graphics::Metal {

	MetalRenderPassBuilder::MetalRenderPassBuilder() {
		m_renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
		m_colorAttachment = std::nullopt;
	}

	MetalRenderPassBuilder::~MetalRenderPassBuilder() =
	default;

	JarRenderPassBuilder* MetalRenderPassBuilder::AddColorAttachment(Graphics::ColorAttachment colorAttachment) {
		m_colorAttachment = std::make_optional(colorAttachment);

		MTL::RenderPassColorAttachmentDescriptor* cd = m_renderPassDescriptor->colorAttachments()->object(0);
		cd->setLoadAction(loadActionToMetal(colorAttachment.loadOp));
		cd->setClearColor(clearColorToMetal(colorAttachment.clearColor));
		cd->setStoreAction(storeActionToMetal(colorAttachment.storeOp));
		return this;
	}

	JarRenderPassBuilder* MetalRenderPassBuilder::AddDepthStencilAttachment(
			Graphics::DepthAttachment depthStencilAttachment) {

		MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = m_renderPassDescriptor->depthAttachment();
		depthAttachment->setLoadAction(loadActionToMetal(depthStencilAttachment.DepthLoadOp));
		depthAttachment->setClearDepth(depthStencilAttachment.DepthClearValue);
		depthAttachment->setStoreAction(storeActionToMetal(depthStencilAttachment.DepthStoreOp));

		m_depthStencilFormat = std::make_optional(pixelFormatMap[depthStencilAttachment.Format]);
		return this;
	}

	JarRenderPassBuilder* MetalRenderPassBuilder::SetMultisamplingCount(uint8_t multisamplingCount) {
		m_multisamplingCount = std::make_optional(multisamplingCount);
		return this;
	}

	std::shared_ptr<JarRenderPass>
	MetalRenderPassBuilder::Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) {
		if (!m_colorAttachment.has_value())
			throw std::exception();

		auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		auto renderPassImages = new MetalRenderPassImages(metalDevice);

		renderPassImages->CreateRenderPassImages(metalSurface->GetSurfaceExtent().Width,
		                                         metalSurface->GetSurfaceExtent().Height,
		                                         m_multisamplingCount.value(),
		                                         metalSurface->getDrawablePixelFormat(),
		                                         m_depthStencilFormat);

		auto colorAttachmentDesc = m_renderPassDescriptor->colorAttachments()->object(0);
		colorAttachmentDesc->setTexture(renderPassImages->getMSAATexture());
		colorAttachmentDesc->setResolveTexture(metalSurface->getDrawable()->texture());
		colorAttachmentDesc->setStoreAction(MTL::StoreActionMultisampleResolve);

		if (m_depthStencilFormat.has_value())
			m_renderPassDescriptor->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
		return std::make_shared<MetalRenderPass>(m_renderPassDescriptor, renderPassImages);
	}
}