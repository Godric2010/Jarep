//
// Created by sebastian on 09.06.24.
//

#include "VulkanImage.hpp"

namespace Graphics::Vulkan {
	VulkanImage::~VulkanImage() = default;

	void VulkanImage::Release() {
		vkDestroySampler(m_device->getLogicalDevice(), m_sampler, nullptr);
		vkDestroyImageView(m_device->getLogicalDevice(), m_imageView, nullptr);
		vkDestroyImage(m_device->getLogicalDevice(), m_image, nullptr);
		vkFreeMemory(m_device->getLogicalDevice(), m_imageMemory, nullptr);
	}
}