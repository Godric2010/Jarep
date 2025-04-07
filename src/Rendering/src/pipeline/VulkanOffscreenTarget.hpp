//
// Created by Godri on 3/30/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

namespace JAREP::Rendering::Pipeline {
	class VulkanOffscreenTarget {
		public:
			VulkanOffscreenTarget(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D resolution,
			                      VkFormat format, VkRenderPass renderPass);

			~VulkanOffscreenTarget();

			VulkanOffscreenTarget(const VulkanOffscreenTarget&) = delete;

			VulkanOffscreenTarget& operator=(const VulkanOffscreenTarget&) = delete;

			VkFramebuffer getFramebuffer() const;

			VkImageView getImageView() const;

			VkImage getImage() const;

			VkExtent2D getExtent() const;

		private:
			void createImage();

			void createImageView();

			void createFramebuffer(VkRenderPass renderPass);

			VkDevice m_device;
			VkPhysicalDevice m_physicalDevice;
			VkExtent2D m_extent;
			VkFormat m_format;

			VkImage m_image;
			VkImageView m_imageView;
			VkFramebuffer m_framebuffer;
			VkDeviceMemory m_memory;
	};
}
