//
// Created by Godri on 3/26/2025.
//

#include "VulkanSampler.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanSampler::VulkanSampler(VkDevice device, VkPhysicalDevice physicalDevice, const SamplerConfig&config) {
	m_device = device;
	m_sampler = VK_NULL_HANDLE;

	SamplerConfig cfg = config;

	if (cfg.anisotropyEnable) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		cfg.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
	}

	if (cfg.unnormalizedCoordinates) {
		cfg.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		cfg.minLod = 0.0f;
		cfg.maxLod = 0.0f;
		cfg.anisotropyEnable = false;
	}

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = cfg.magFilter;
	samplerInfo.minFilter = cfg.minFilter;

	samplerInfo.addressModeU = cfg.addressModeU;
	samplerInfo.addressModeV = cfg.addressModeV;
	samplerInfo.addressModeW = cfg.addressModeW;

	samplerInfo.anisotropyEnable = cfg.anisotropyEnable ? VK_TRUE : VK_FALSE;
	samplerInfo.maxAnisotropy = cfg.maxAnisotropy;

	samplerInfo.borderColor = cfg.borderColor;
	samplerInfo.unnormalizedCoordinates = cfg.unnormalizedCoordinates ? VK_TRUE : VK_FALSE;
	samplerInfo.compareEnable = cfg.compareEnable ? VK_TRUE : VK_FALSE;
	samplerInfo.compareOp = cfg.compareOp;
	samplerInfo.mipmapMode = cfg.mipmapMode;
	samplerInfo.mipLodBias = cfg.mipLodBias;
	samplerInfo.minLod = cfg.minLod;
	samplerInfo.maxLod = cfg.maxLod;

	if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create sampler!");
	}
}

VulkanSampler::~VulkanSampler() {
	if (m_sampler != VK_NULL_HANDLE) {
		vkDestroySampler(m_device, m_sampler, nullptr);
	}
}

VkSampler VulkanSampler::get() const {
	return m_sampler;
}
