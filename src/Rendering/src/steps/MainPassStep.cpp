//
// Created by Godri on 3/30/2025.
//

#include "MainPassStep.hpp"

#include <iostream>

using namespace JAREP::Rendering::Steps;

MainPassStep::MainPassStep(VkDevice device, VkPhysicalDevice physicalDevice) {
	m_device = device;
	m_physicalDevice = physicalDevice;
	m_pipelineLayout = VK_NULL_HANDLE;
	m_format = {};
	m_extent = {};
} ;

MainPassStep::~MainPassStep() {
	m_offscreenTarget.reset();
	m_pipeline.reset();
	if (m_pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	}
	m_renderPass.reset();
}


void MainPassStep::Prepare(const VkExtent2D extent, const VkFormat format) {
	m_extent = extent;
	m_format = format;

	createRenderPass();
	createPipeline();
	createFramebuffer();
}

void MainPassStep::Resize(VkExtent2D newExtent) {
	m_extent = newExtent;
	m_offscreenTarget.reset();
	m_pipeline.reset();
	m_renderPass.reset();

	createRenderPass();
	createPipeline();
	createFramebuffer();
}

void MainPassStep::Record(VkCommandBuffer cmdBuffer) {

	vkResetCommandBuffer(cmdBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->get();
	renderPassInfo.framebuffer = m_offscreenTarget->getFramebuffer();
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = m_extent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	std::cout << "Framebuffer extent {W: " << m_extent.width << " H:" << m_extent.height
			<< "}" << std::endl;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_extent.width);
	viewport.height = static_cast<float>(m_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.extent = m_extent;
	scissor.offset = {0, 0};
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());
	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(cmdBuffer);

	if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

VkImageView MainPassStep::GetOutput() {
	return m_offscreenTarget->getImageView();
}

void MainPassStep::createRenderPass() {
	Pipeline::RenderPassConfig config = {
		.colorFormat = m_format,
		.depthFormat = std::nullopt,
		.samples = VK_SAMPLE_COUNT_1_BIT,
	};

	m_renderPass = std::make_unique<Pipeline::VulkanRenderPass>(m_device, config);
}

void MainPassStep::createPipeline() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
	    VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	Pipeline::VulkanPipelineConfig config = {
		.device = m_device,
		.renderPass = m_renderPass->get(),
		.extent = m_extent,
		.vertexShaderPath = "triangle.vert.spv",
		.fragmentShaderPath = "triangle.frag.spv",
		.pipelineLayout = m_pipelineLayout,
		.depthFormat = std::nullopt,
		// .depthTestEnable = false,
		// .depthWriteEnable = false,
	};

	m_pipeline = std::make_unique<Pipeline::VulkanPipeline>(config);
}

void MainPassStep::createFramebuffer() {
	m_offscreenTarget = std::make_unique<Pipeline::VulkanOffscreenTarget>(
		m_device, m_physicalDevice, m_extent, m_format, m_renderPass->get());
}
