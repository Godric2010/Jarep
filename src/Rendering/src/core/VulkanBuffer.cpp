//
// Created by Godri on 3/25/2025.
//

#include "VulkanBuffer.hpp"

using namespace JAREP::Rendering::Core;

VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                           VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	m_device = device;
	m_size = size;
	m_memoryProperties = properties;
	m_memory = VK_NULL_HANDLE;
	m_buffer = VK_NULL_HANDLE;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, m_buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate memory");
	}

	vkBindBufferMemory(device, m_buffer, m_memory, 0);
}

VulkanBuffer::~VulkanBuffer() {
	if (m_memory != VK_NULL_HANDLE) {
		vkFreeMemory(m_device, m_memory, nullptr);
	}
	if (m_buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(m_device, m_buffer, nullptr);
	}
}

VkBuffer VulkanBuffer::getBuffer() const {
	return m_buffer;
}

VkDeviceMemory VulkanBuffer::getMemory() const {
	return m_memory;
}

VkDeviceSize VulkanBuffer::getSize() const {
	return m_size;
}

void* VulkanBuffer::map(VkDeviceSize offset, VkDeviceSize size) const {
	if (!(m_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		throw std::runtime_error("Tried to map non-host-visible memory!");
	}

	void* data;
	if (vkMapMemory(m_device, m_memory, offset, size, 0, &data) != VK_SUCCESS) {
		throw std::runtime_error("failed to map vulkan buffer memory");
	}

	return data;
}

void VulkanBuffer::unmap() const {
	if (!(m_memoryProperties * VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		throw std::runtime_error("Tried to unmap non-host-visible memory!");
	}
	vkUnmapMemory(m_device, m_memory);
}

void VulkanBuffer::copyFrom(const void* srcData, VkDeviceSize size, VkDeviceSize offset) const {
	void* dst = map(offset, size);
	std::memcpy(dst, srcData, static_cast<size_t>(size));
	unmap();
}

bool VulkanBuffer::isHostVisible() const {
	return m_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

bool VulkanBuffer::isDeviceLocal() const {
	return m_memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
}

uint32_t VulkanBuffer::findMemoryType(const VkPhysicalDevice physicalDevice, const uint32_t typeFilter,
                                      const VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type");
}

void VulkanBuffer::copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer src, VkBuffer dst,
                              VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
