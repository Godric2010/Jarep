//
// Created by Godri on 3/25/2025.
//

#include "VulkanCommandPool.hpp"

using namespace JAREP::Rendering::Core;

VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex) {
	m_device = device;
	m_commandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_device, &cmdPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

VulkanCommandPool::~VulkanCommandPool() {
	if (m_commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}
}

VkCommandPool VulkanCommandPool::get() const {
	return m_commandPool;
}

std::vector<VkCommandBuffer>
VulkanCommandPool::allocateCommandBuffers(const uint32_t count, const VkCommandBufferLevel level) const {
	std::vector<VkCommandBuffer> commandBuffers(count);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = level;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = count;

	if (vkAllocateCommandBuffers(m_device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	return commandBuffers;
}

VkCommandBuffer VulkanCommandPool::beginSingleTimeCommands() const {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	if (vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin command buffer submit!");
	}
	return commandBuffer;
}

void VulkanCommandPool::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue) const {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to end command buffer submit!");
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit command buffer submit!");
	}

	vkQueueWaitIdle(queue);
	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}
