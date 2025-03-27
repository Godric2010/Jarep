//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Core {
	/**
	 * Handles th binding of actual resources (buffers, images) to a descriptor set layout.
	 */
	class VulkanDescriptorSet {
		public:
			VulkanDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool,
			                    VkDescriptorSetLayout layout);

			~VulkanDescriptorSet() = default;

			VkDescriptorSet get() const;

			void bindUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize size, VkDeviceSize offset = 0);

			void bindCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler,
			                              VkImageLayout layout);

		private:
			VkDevice m_device;
			VkDescriptorSet m_descriptorSet;
	};
}
