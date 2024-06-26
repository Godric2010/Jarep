//
// Created by sebastian on 09.06.24.
//

#include "VulkanImage.hpp"

namespace Graphics::Vulkan {
	VulkanImage::~VulkanImage() = default;

	void VulkanImage::Release() {
		vkDestroySampler(m_device->GetLogicalDevice(), m_sampler, nullptr);
		m_imageBuffer->Release();
	}
}