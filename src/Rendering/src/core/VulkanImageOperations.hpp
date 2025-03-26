//
// Created by Godri on 3/26/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Core {
	void transitionImageLayout(VkDevice device,
	                           VkCommandPool commandPool,
	                           VkQueue queue,
	                           VkImage image,
	                           VkFormat format,
	                           VkImageLayout oldLayout,
	                           VkImageLayout newLayout,
	                           VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

	void copyBufferToImage(VkDevice device,
	                       VkCommandPool commandPool,
	                       VkQueue queue,
	                       VkBuffer srcBuffer,
	                       VkImage dstImage,
	                       uint32_t width,
	                       uint32_t height);

	void generateMipmaps(VkDevice device,
	                     VkPhysicalDevice physicalDevice,
	                     VkCommandPool commandPool,
	                     VkQueue queue,
	                     VkImage image,
	                     VkFormat format,
	                     uint32_t texWidth,
	                     uint32_t texHeight,
	                     uint32_t mipLevels);
}
