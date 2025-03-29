//
// Created by Godri on 3/25/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <cstdint>

namespace JAREP::Rendering::Core {
	/**
	 * Abstraction for Vulkan buffer objects. Handles creation, memory allocation and data transfer.
	 */
	class VulkanBuffer {
		public:
			VulkanBuffer(VkDevice device,
			             VkPhysicalDevice physicalDevice,
			             VkDeviceSize size,
			             VkBufferUsageFlags usage,
			             VkMemoryPropertyFlags properties);

			~VulkanBuffer();

			VulkanBuffer(const VulkanBuffer&) = delete;

			VulkanBuffer& operator=(const VulkanBuffer&) = delete;

			VkBuffer getBuffer() const;

			VkDeviceMemory getMemory() const;

			VkDeviceSize getSize() const;

			void* map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE) const;

			void unmap() const;

			void copyFrom(const void* srcData, VkDeviceSize size, VkDeviceSize offset = 0) const;


			bool isHostVisible() const;

			bool isDeviceLocal() const;

			static void copyBuffer(VkDevice device,
			                       VkCommandPool commandPool,
			                       VkQueue queue,
			                       VkBuffer src,
			                       VkBuffer dst,
			                       VkDeviceSize size);

		private:
			VkDevice m_device;
			VkDeviceMemory m_memory;
			VkDeviceSize m_size;
			VkBuffer m_buffer;
			VkMemoryPropertyFlags m_memoryProperties;

			uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
			                        VkMemoryPropertyFlags properties) const;
	};
}
