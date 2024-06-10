//
// Created by sebastian on 09.06.24.
//

#include "VulkanGraphicsPipeline.hpp"

namespace Graphics::Vulkan {
	VulkanGraphicsPipeline::~VulkanGraphicsPipeline() = default;

	std::shared_ptr<JarRenderPass> VulkanGraphicsPipeline::GetRenderPass() {
		return m_renderPass;
	}

	void VulkanGraphicsPipeline::Release() {

		m_renderPass->Release();
		vkDestroyPipelineLayout(m_device->getLogicalDevice(), m_pipelineLayout, nullptr);
		vkDestroyPipeline(m_device->getLogicalDevice(), m_graphicsPipeline, nullptr);
	}
}