//
// Created by Godri on 4/9/2025.
//

#include "VulkanDepthBuffer.hpp"

#include <stdexcept>
#include <vector>

using namespace JAREP::Rendering::Pipeline;

VulkanDepthBuffer::VulkanDepthBuffer(const VkDevice device,
                                     const VkPhysicalDevice physicalDevice,
                                     const VkExtent2D extent,
                                     const VkSampleCountFlagBits sampleCountFlagBits,
                                     const bool enableStencil) {
	m_device = device;

	m_format = VK_FORMAT_UNDEFINED;

	findDepthFormat(physicalDevice, enableStencil);
	createImage(physicalDevice, extent, sampleCountFlagBits, enableStencil);
}

VulkanDepthBuffer::~VulkanDepthBuffer() {
	m_depthImage.reset();
}

VkImage VulkanDepthBuffer::getDepthImage() const {
	return m_depthImage->getImage();
}


VkFormat VulkanDepthBuffer::getFormat() const {
	return m_format;
}

VkImageView VulkanDepthBuffer::getImageView() const {
	return m_depthImage->getImageView();
}

void VulkanDepthBuffer::findDepthFormat(VkPhysicalDevice physicalDevice, bool enableStencil) {
	std::vector<VkFormat> formats;
	if (enableStencil) {
		formats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	}
	else {
		formats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM};
	}

	for (VkFormat format: formats) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
		if ((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
		    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			m_format = format;
			return;
		}
		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
		    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			m_format = format;
			return;
		}
	}

	throw std::runtime_error("Could not find a valid depth format!");
}


void VulkanDepthBuffer::createImage(VkPhysicalDevice physicalDevice, VkExtent2D extent,
                                    VkSampleCountFlagBits sampleCountFlagBits, const bool enableStencil) {
	int aspectMask;
	if (enableStencil) {
		aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else {
		aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	m_depthImage = std::make_unique<Core::VulkanImage>(m_device,
	                                                   physicalDevice,
	                                                   extent.width, extent.height,
	                                                   m_format,
	                                                   VK_IMAGE_TILING_OPTIMAL,
	                                                   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	                                                   aspectMask,
	                                                   sampleCountFlagBits);
}
