//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Core {
	class VulkanDescriptorPool {
		public:
			struct PoolSize {
				VkDescriptorType type;
				uint32_t count;
			};

			VulkanDescriptorPool(VkDevice device, const std::vector<PoolSize>&sizes, uint32_t maxSets);

			~VulkanDescriptorPool();

			VkDescriptorPool get() const;

			void reset() const;

		private:
			VkDevice m_device;
			VkDescriptorPool m_descriptorPool;
	};
}
