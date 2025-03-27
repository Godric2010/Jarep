//
// Created by Godri on 3/27/2025.
//

#include "VulkanDescriptorSet.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanDescriptorSet::VulkanDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool,
                                         VkDescriptorSetLayout layout) {
	m_device = device;
	m_descriptorSet = VK_NULL_HANDLE;

	VkDescriptorSetLayout layouts[] = {layout};

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = layouts;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}
}


VkDescriptorSet VulkanDescriptorSet::get() const {
	return m_descriptorSet;
}

void VulkanDescriptorSet::bindUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize size, VkDeviceSize offset) {
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer;
	bufferInfo.range = size;
	bufferInfo.offset = offset;

	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = m_descriptorSet;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.descriptorCount = 1;
	write.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}

void VulkanDescriptorSet::bindCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout layout) {

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageView = imageView;
	imageInfo.sampler = sampler;
	imageInfo.imageLayout = layout;

	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = m_descriptorSet;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.descriptorCount = 1;
	write.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}

