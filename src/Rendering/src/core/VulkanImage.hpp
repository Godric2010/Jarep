//
// Created by Godri on 3/25/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Core {
	/**
	 * Represents a Vulkan image object (textures, render tragets). Manages memory and layout transitions.
	 */
	class VulkanImage {
		public:
			VulkanImage(VkDevice device,
			            VkPhysicalDevice physicalDevice,
			            uint32_t width,
			            uint32_t height,
			            VkFormat format,
			            VkImageTiling tiling,
			            VkImageUsageFlags usage,
			            VkMemoryPropertyFlags properties,
			            VkImageAspectFlags aspect,
			            VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

			~VulkanImage();

			VulkanImage(const VulkanImage&) = delete;

			VulkanImage& operator=(const VulkanImage&) = delete;

			VkImage getImage() const;

			VkImageView getImageView() const;

			VkDeviceMemory getMemory() const;

		private:
			VkDevice m_device;
			VkImage m_image;
			VkDeviceMemory m_imageMemory;
			VkImageView m_imageView;

			uint32_t findMemoryType(VkPhysicalDevice physicalDevice,
			                        uint32_t typeFilter,
			                        VkMemoryPropertyFlags properties) const;
	};
}
