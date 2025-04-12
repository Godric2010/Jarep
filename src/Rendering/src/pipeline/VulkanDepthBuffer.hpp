//
// Created by Godri on 4/9/2025.
//

#pragma once
#include <memory>
#include <vulkan/vulkan_core.h>
#include "../core/VulkanImage.hpp"

namespace JAREP::Rendering::Pipeline {
	class VulkanDepthBuffer {
		public:
			VulkanDepthBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent,
			                  VkSampleCountFlagBits sampleCountFlagBits, bool enableStencil);

			~VulkanDepthBuffer();

			VulkanDepthBuffer(const VulkanDepthBuffer&) = delete;

			VulkanDepthBuffer& operator=(const VulkanDepthBuffer&) = delete;

			VkImage getDepthImage() const;

			VkImageView getImageView() const;

			VkFormat getFormat() const;

		private:
			void findDepthFormat(VkPhysicalDevice physicalDevice, bool enableStencil);

			void createImage(VkPhysicalDevice physicalDevice, VkExtent2D extent,
			                 VkSampleCountFlagBits sampleCountFlagBits, bool enableStencil);

			VkDevice m_device;
			VkFormat m_format;

			std::unique_ptr<Core::VulkanImage> m_depthImage;
			VkDevice device_;
	};
}
