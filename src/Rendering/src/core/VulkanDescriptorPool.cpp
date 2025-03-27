//
// Created by Godri on 3/27/2025.
//

#include "VulkanDescriptorPool.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device, const std::vector<PoolSize>&sizes,
                                           uint32_t maxSets) {
	m_device = device;
	m_descriptorPool = VK_NULL_HANDLE;

	std::vector<VkDescriptorPoolSize> vkSizes;
	vkSizes.reserve(sizes.size());

	for (const auto&size: sizes) {
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = size.type;
		poolSize.descriptorCount = size.count;
		vkSizes.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets = maxSets;
	poolInfo.poolSizeCount = static_cast<uint32_t>(vkSizes.size());
	poolInfo.pPoolSizes = vkSizes.data();
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
	if (m_descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
	}
}

VkDescriptorPool VulkanDescriptorPool::get() const {
	return m_descriptorPool;
}

void VulkanDescriptorPool::reset() const {
	if (vkResetDescriptorPool(m_device, m_descriptorPool, 0) != VK_SUCCESS) {
		throw std::runtime_error("failed to reset descriptor pool!");
	}
}
