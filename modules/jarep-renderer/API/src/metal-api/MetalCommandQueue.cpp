//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalCommandQueue.hpp"

namespace Graphics::Metal {
	MetalCommandQueue::~MetalCommandQueue() = default;

	JarCommandBuffer* MetalCommandQueue::getNextCommandBuffer() {
		return new MetalCommandBuffer(queue->commandBuffer());
	}

	MetalCommandBuffer MetalCommandQueue::GetCommandBuffer() {
		return MetalCommandBuffer(queue->commandBuffer());
	}

	void MetalCommandQueue::Release() {
		queue->release();
	}
}