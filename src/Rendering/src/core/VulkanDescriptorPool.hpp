//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Core {
	/**
	 * Manages the allocation of descriptor sets. Pre-allocates memory for various descriptor types.
	 */
	class VulkanDescriptorPool {
		public:
			struct PoolSize {
				VkDescriptorType type;
				uint32_t count;
			};

			VulkanDescriptorPool(VkDevice device, const std::vector<PoolSize>&sizes, uint32_t maxSets);

			~VulkanDescriptorPool();

			VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;

			VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

			VkDescriptorPool get() const;

			void reset() const;

		private:
			VkDevice m_device;
			VkDescriptorPool m_descriptorPool;
	};
}
