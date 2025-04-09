//
// Created by Godri on 4/9/2025.
//

#pragma once
#include <vulkan/vulkan_core.h>

namespace JAREP::Rendering::Pipeline {
	class VulkanDepthBuffer {
		public:
			VulkanDepthBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent, bool enableStencil);

			~VulkanDepthBuffer();

			VulkanDepthBuffer(const VulkanDepthBuffer&) = delete;

			VulkanDepthBuffer& operator=(const VulkanDepthBuffer&) = delete;

			VkImage getDepthImage() const;

			VkImageView getImageView() const;

			VkFormat getFormat() const;

		private:

			void findDepthFormat(VkPhysicalDevice physicalDevice, bool enableStencil);

			void createImage(VkPhysicalDevice physicalDevice, VkExtent2D extent);

			void createImageView(bool enableStencil);

			VkDevice m_device;

			VkFormat m_format;
			VkImage m_depthImage;
			VkImageView m_depthImageView;
			VkDeviceMemory m_depthImageMemory;
	};
}
