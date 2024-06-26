//
// Created by sebastian on 09.06.24.
//

#include "VulkanBuffer.hpp"
namespace Graphics::Vulkan {
	VulkanBuffer::~VulkanBuffer() = default;

	void VulkanBuffer::Release() {
		vkFreeMemory(m_device->GetLogicalDevice(), m_bufferMemory, nullptr);
		vkDestroyBuffer(m_device->GetLogicalDevice(), m_buffer, nullptr);

		m_bufferReleasedCallback();
	}

	void VulkanBuffer::Update(const void* data, size_t bufferSize) {
		memcpy(m_data, data, bufferSize);
		m_bufferSize = bufferSize;
	}
}