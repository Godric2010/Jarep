//
// Created by sebastian on 09.06.24.
//

#include "VulkanCommandQueue.hpp"
namespace Graphics::Vulkan {
	VulkanCommandQueue::~VulkanCommandQueue() = default;


	JarCommandBuffer* VulkanCommandQueue::getNextCommandBuffer() {
		VulkanCommandBuffer* bufferInFlight = m_commandBuffers[m_currentBufferIndexInUse];

		m_currentBufferIndexInUse = (m_currentBufferIndexInUse + 1) % m_commandBuffers.size();

		return reinterpret_cast<JarCommandBuffer*>(bufferInFlight);
	}

	void VulkanCommandQueue::Release() {

		for (auto& m_commandBuffer: m_commandBuffers) {
			m_commandBuffer->Release(m_device->getLogicalDevice());
		}
		vkDestroyCommandPool(m_device->getLogicalDevice(), m_commandPool, nullptr);
	}
}