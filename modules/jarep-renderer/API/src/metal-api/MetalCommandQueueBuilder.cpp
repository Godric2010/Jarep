//
// Created by Sebastian Borsch on 22.05.24.
//

#include "MetalCommandQueueBuilder.hpp"
namespace Graphics::Metal {
	MetalCommandQueueBuilder::~MetalCommandQueueBuilder() = default;

	MetalCommandQueueBuilder* MetalCommandQueueBuilder::SetCommandBufferAmount(uint32_t commandBufferAmount) {
		m_amountOfCommandBuffers = std::make_optional(commandBufferAmount);
		return this;
	}

	std::shared_ptr<JarCommandQueue> MetalCommandQueueBuilder::Build(std::shared_ptr<JarDevice> device) {
		const auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		uint32_t commandBuffersCount = m_amountOfCommandBuffers.value_or(DEFAULT_COMMAND_BUFFER_COUNT);
		MTL::Device* mtlDevice = metalDevice->getDevice();
		MTL::CommandQueue* commandQueue = mtlDevice->newCommandQueue(static_cast<NS::UInteger>(commandBuffersCount));
		return std::make_shared<MetalCommandQueue>(commandQueue);
	}
}