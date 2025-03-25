//
// Created by Godri on 3/25/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Core {
	class VulkanCommandPool {
		public:
			VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex);

			~VulkanCommandPool();

			VkCommandPool get() const;

			std::vector<VkCommandBuffer> allocateCommandBuffers(uint32_t count, VkCommandBufferLevel level =
					                                                    VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

			VkCommandBuffer beginSingleTimeCommands() const;

			void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue) const;

		private:
			VkDevice m_device;
			VkCommandPool m_commandPool;
	};
}
