//
// Created by Godri on 3/28/2025.
//

#include "VulkanRenderPass.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Pipeline;

VulkanRenderPass::VulkanRenderPass(VkDevice device, const RenderPassConfig&config) {
	m_device = device;
	m_config = config;
	m_renderPass = VK_NULL_HANDLE;

	createRenderPass(config.outputLayout);
}

VulkanRenderPass::~VulkanRenderPass() {
	if (m_renderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	}
}

VkRenderPass VulkanRenderPass::get() const {
	return m_renderPass;
}

void VulkanRenderPass::createRenderPass(VkImageLayout outputLayout) {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_config.colorFormat;
	colorAttachment.samples = m_config.samples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = outputLayout;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentDescription> attachments = {colorAttachment};
	std::vector<VkAttachmentReference> colorAttachmentReferences = {colorAttachmentRef};

	VkAttachmentReference depthStencilAttachmentRef = {};
	if (m_config.depthFormat.has_value()) {
		VkAttachmentDescription depthStencilAttachment = {};
		depthStencilAttachment.format = m_config.depthFormat.value();
		depthStencilAttachment.samples = m_config.samples;
		depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthStencilAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthStencilAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthStencilAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthStencilAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		depthStencilAttachmentRef.attachment = static_cast<uint32_t>(attachments.size());
		depthStencilAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments.push_back(depthStencilAttachment);
	}

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
	subpass.pColorAttachments = colorAttachmentReferences.data();
	if (m_config.depthFormat.has_value()) {
		subpass.pDepthStencilAttachment = &depthStencilAttachmentRef;
	}
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}
