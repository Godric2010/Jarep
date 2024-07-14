//
// Created by Sebastian Borsch on 07.07.24.
//

#include "MetalImageBuffer.hpp"

namespace Graphics::Metal {

	MetalImageBuffer::MetalImageBuffer(std::shared_ptr<MetalDevice> device, MTL::Texture* texture,
	                                   std::function<std::shared_ptr<MetalCommandQueue>()> getCommandQueueCb)
			: m_device(device), m_texture(texture), m_getCommandQueueCb(getCommandQueueCb) {
	}

	MetalImageBuffer::~MetalImageBuffer() {
	}

	void MetalImageBuffer::Release() {
		m_texture = nullptr;
	}

	void MetalImageBuffer::UploadData(const void* data, size_t bufferSize) {
		if (!m_texture) {
			throw std::runtime_error("MetalImageBuffer: UploadData() called without setting texture");
		}
		auto buffer = m_device->getDevice()->newBuffer(data, bufferSize, MTL::ResourceStorageModeShared);
		auto commandBuffer = m_getCommandQueueCb()->GetCommandBuffer().getCommandBuffer();
		auto blitEncoder = commandBuffer->blitCommandEncoder();
		blitEncoder->copyFromBuffer(buffer, 0, 0, 0, MTL::Size{m_texture->width(), m_texture->height(), 1}, m_texture,
		                            0, 0, MTL::Origin{0, 0, 0});
		blitEncoder->endEncoding();
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		buffer->release();

	}

	MTL::Texture* MetalImageBuffer::GetTexture() const {
		return m_texture;
	}
}