//
// Created by sebastian on 09.06.24.
//

#include "VulkanShaderModule.hpp"

namespace Graphics::Vulkan {
	VulkanShaderModule::~VulkanShaderModule() = default;

	void VulkanShaderModule::Release() {
		vkDestroyShaderModule(m_device->GetLogicalDevice(), m_shaderModule, nullptr);
	}
}