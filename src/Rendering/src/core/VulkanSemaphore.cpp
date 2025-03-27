//
// Created by Godri on 3/27/2025.
//

#include "VulkanSemaphore.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanSemaphore::VulkanSemaphore(VkDevice device) {
	m_device = device;
	m_semaphore = VK_NULL_HANDLE;

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_semaphore) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphore!");
	}
}

VulkanSemaphore::~VulkanSemaphore() {
	if (m_semaphore != VK_NULL_HANDLE) {
		vkDestroySemaphore(m_device, m_semaphore, nullptr);
	}
}

VkSemaphore VulkanSemaphore::get() const {
	return m_semaphore;
}
