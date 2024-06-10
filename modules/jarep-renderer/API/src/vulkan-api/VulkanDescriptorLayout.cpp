//
// Created by sebastian on 09.06.24.
//

#include "VulkanDescriptorLayout.hpp"

namespace Graphics::Vulkan {
	VulkanDescriptorLayout::~VulkanDescriptorLayout() = default;

	void VulkanDescriptorLayout::Release() {
		vkDestroyDescriptorSetLayout(m_device->getLogicalDevice(), m_descriptorSetLayout, nullptr);
	}
}