//
// Created by Godri on 3/27/2025.
//

#include "VulkanDescriptorSetLayout.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, const std::vector<BindingInfo>& bindings) {

	m_device = device;
	m_layout = VK_NULL_HANDLE;

	std::vector<VkDescriptorSetLayoutBinding> vkBindings;
	vkBindings.resize(bindings.size());

	for (const auto& binding: bindings) {
		VkDescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = binding.binding;
		layoutBinding.descriptorType = binding.type;
		layoutBinding.descriptorCount = binding.count;
		layoutBinding.stageFlags = binding.stageFlags;
		layoutBinding.pImmutableSamplers = nullptr;
		vkBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(vkBindings.size());
	layoutInfo.pBindings = vkBindings.data();

	if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
	if (m_layout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
	}
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::get() const {
	return m_layout;
}

