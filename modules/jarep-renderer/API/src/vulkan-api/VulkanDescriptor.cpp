//
// Created by sebastian on 09.06.24.
//

#include "VulkanDescriptor.hpp"

namespace Graphics::Vulkan {
	VulkanDescriptor::~VulkanDescriptor() = default;

	void VulkanDescriptor::Release() {
		vkDestroyDescriptorPool(m_device->getLogicalDevice(), m_descriptorPool, nullptr);
		m_descriptorSetLayout->Release();
	}

	std::shared_ptr<JarDescriptorLayout> VulkanDescriptor::GetDescriptorLayout() {
		return m_descriptorSetLayout;
	}

	VkDescriptorSet VulkanDescriptor::GetNextDescriptorSet() {

		auto descriptorSet = m_descriptorSets[m_descriptorSetIndex];
		m_descriptorSetIndex = (m_descriptorSetIndex + 1) % m_descriptorSets.size();
		return descriptorSet;
	}

}