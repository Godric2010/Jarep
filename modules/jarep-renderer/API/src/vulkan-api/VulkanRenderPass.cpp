//
// Created by sebastian on 09.06.24.
//

#include "VulkanRenderPass.hpp"

namespace Graphics::Vulkan {
	VulkanRenderPass::~VulkanRenderPass() = default;

	void VulkanRenderPass::Release() {
		vkDestroyRenderPass(m_device->GetLogicalDevice(), m_renderPass, nullptr);
	}
}