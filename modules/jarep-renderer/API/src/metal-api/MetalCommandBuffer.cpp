//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalCommandBuffer.hpp"

namespace Graphics::Metal{
	MetalCommandBuffer::~MetalCommandBuffer() = default;

	bool
	MetalCommandBuffer::StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) {
		metalRenderPass = reinterpret_cast<std::shared_ptr<MetalRenderPass>&>(renderPass);
		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);

		auto renderPassDesc = metalRenderPass->getRenderPassDesc();
		renderPassDesc->colorAttachments()->object(0)->setTexture(metalSurface->acquireNewDrawTexture());
		encoder = buffer->renderCommandEncoder(renderPassDesc);
		return true;
	}

	void MetalCommandBuffer::EndRecording() {
		encoder->endEncoding();
	}

	void MetalCommandBuffer::BindPipeline(std::shared_ptr<Graphics::JarPipeline> pipeline, uint32_t frameIndex) {
		auto metalPipeline = reinterpret_cast<MetalPipeline*>(pipeline.get());
		encoder->setRenderPipelineState(metalPipeline->getPSO());

		auto depthStencilState = metalPipeline->getDSS();
		if (depthStencilState.has_value())
			encoder->setDepthStencilState(depthStencilState.value());

		encoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);
		encoder->setCullMode(MTL::CullMode::CullModeBack);
	}

	void MetalCommandBuffer::BindDescriptors(std::vector<std::shared_ptr<JarDescriptor>> descriptors) {
		auto metalDescriptors = reinterpret_cast<std::vector<std::shared_ptr<MetalDescriptor>>&>(descriptors);
		for (auto& descriptor: metalDescriptors) {
			descriptor->BindContentToEncoder(encoder);
		}
	}

	void MetalCommandBuffer::BindVertexBuffer(std::shared_ptr<Graphics::JarBuffer> jarBuffer) {
		auto* metalBuffer = reinterpret_cast<MetalBuffer*>(jarBuffer.get());
		encoder->setVertexBuffer(metalBuffer->getBuffer().value(), 0, 0);
	}

	void MetalCommandBuffer::BindIndexBuffer(std::shared_ptr<JarBuffer> jarBuffer) {
		std::shared_ptr<MetalBuffer> metalBuffer = reinterpret_cast<std::shared_ptr<MetalBuffer>&>(jarBuffer);
		indexBuffer = metalBuffer;
	}

	void MetalCommandBuffer::SetDepthBias(Graphics::DepthBias depthBias) {
		encoder->setDepthBias(depthBias.DepthBiasConstantFactor, depthBias.DepthBiasClamp,
		                      depthBias.DepthBiasSlopeFactor);
	}

	void MetalCommandBuffer::SetViewport(Graphics::Viewport viewport) {
		auto metalViewport = MTL::Viewport();
		metalViewport.height = viewport.height;
		metalViewport.width = viewport.width;
		metalViewport.originX = viewport.x;
		metalViewport.originY = viewport.y;
		metalViewport.znear = viewport.minDepth;
		metalViewport.zfar = viewport.maxDepth;
		encoder->setViewport(metalViewport);
	}

	void MetalCommandBuffer::SetScissor(Graphics::Scissor scissor) {
		auto metalScissor = MTL::ScissorRect();
		metalScissor.x = scissor.x;
		metalScissor.y = scissor.y;
		metalScissor.width = scissor.width;
		metalScissor.height = scissor.height;
		encoder->setScissorRect(metalScissor);
	}

	void MetalCommandBuffer::DrawIndexed(size_t indexAmount) {
		encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(indexAmount), MTL::IndexTypeUInt16,
		                               indexBuffer->getBuffer().value(), 0);
	}

	void MetalCommandBuffer::Draw() {
		encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
	}

	void MetalCommandBuffer::Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) {
		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);

		metalRenderPass->UpdateRenderPassDescriptor(metalSurface);

		buffer->presentDrawable(metalSurface->getDrawable());
		buffer->commit();
		buffer->waitUntilCompleted();
	}
}