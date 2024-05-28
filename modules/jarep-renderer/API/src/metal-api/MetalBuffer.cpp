//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalBuffer.hpp"

namespace Graphics::Metal {

	MetalBuffer::~MetalBuffer() = default;

	void MetalBuffer::Release() {
		m_buffer->release();
	}

	void MetalBuffer::Update(const void* data, size_t bufferSize) {
		memcpy(m_buffer->contents(), data, bufferSize);
		m_buffer->didModifyRange(NS::Range::Make(0, m_buffer->length()));
	}

	std::optional<MTL::Buffer*> MetalBuffer::getBuffer() {
		if (m_buffer == nullptr) return std::nullopt;
		return std::make_optional(m_buffer);
	}
}