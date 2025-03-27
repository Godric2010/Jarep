//
// Created by Godri on 3/27/2025.
//

#include "VulkanFence.hpp"
#include<stdexcept>

using namespace JAREP::Rendering::Core;

VulkanFence::VulkanFence(VkDevice device, bool signaled) {
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	if (vkCreateFence(m_device, &fenceInfo, nullptr, &m_fence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create fence");
	}
}

VulkanFence::~VulkanFence() {
	if (m_fence != VK_NULL_HANDLE) {
		vkDestroyFence(m_device, m_fence, nullptr);
	}
}

VkFence VulkanFence::get() const {
	return m_fence;
}

void VulkanFence::wait(uint64_t timeout) const {
	vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, timeout);
}

void VulkanFence::reset() const {
	vkResetFences(m_device, 1, &m_fence);
}

bool VulkanFence::isSignaled() const {
	return vkGetFenceStatus(m_device, m_fence) == VK_SUCCESS;
}
