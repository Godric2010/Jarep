//
// Created by Godri on 3/30/2025.
//

#include "MainPassStep.hpp"

#include <iostream>

#include "../VulkanVertexLayout.hpp"
#include "../core/VulkanCore.hpp"
#include "../pipeline/VulkanDepthBuffer.hpp"

using namespace JAREP::Rendering::Steps;

MainPassStep::MainPassStep(VkDevice device, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits sampleCountFlags,
                           Meshes::VulkanMeshRegistry* meshRegistry) {
	m_device = device;
	m_physicalDevice = physicalDevice;
	m_sampleCountFlag = sampleCountFlags;
	m_pipelineLayout = VK_NULL_HANDLE;
	m_format = {};
	m_extent = {};
	m_meshRegistry = meshRegistry;
	m_objectUBO = std::make_unique<Core::VulkanUniformBuffer<Core::ObjectUBO>>(device, physicalDevice);
} ;

MainPassStep::~MainPassStep() {
	m_meshRegistry = nullptr;
	m_objectUBO.reset();
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

	createDepthImage();
	createRenderPass();
	createPipeline();
	createFramebuffer();
}

void MainPassStep::BindToOutputOf(IRenderStep* previousRenderStep) {
	throw std::runtime_error("Not implemented for main pass step!");
}

void MainPassStep::Resize(VkExtent2D newExtent) {
	// m_extent = newExtent;
	// m_offscreenTarget.reset();
	// m_pipeline.reset();
	// m_renderPass.reset();
	//
	// createRenderPass();
	// createPipeline();
	// createFramebuffer();
}

void MainPassStep::Record(VkCommandBuffer cmdBuffer) {
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->get();
	renderPassInfo.framebuffer = m_offscreenTarget->getFramebuffer();
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = m_extent;

	std::vector<VkClearValue> clearValues;
	VkClearValue clearColor = {};
	clearColor.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues.push_back(clearColor);

	if (m_sampleCountFlag != VK_SAMPLE_COUNT_1_BIT) {
		VkClearValue clearResolve = {};
		clearValues.push_back(clearResolve);
	}

	VkClearValue clearDepth = {};
	clearDepth.depthStencil = {1.0f, 0};
	clearValues.push_back(clearDepth);

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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

	for (const auto&renderObject: m_renderObjects) {
		auto mesh = m_meshRegistry->TryGetMesh(renderObject.meshID);
		if (!mesh.has_value()) continue;

		m_objectUBO->Update(renderObject.transform);
		mesh.value()->Bind(cmdBuffer);
		mesh.value()->Draw(cmdBuffer);
	}

	vkCmdEndRenderPass(cmdBuffer);
}

VkImageView MainPassStep::GetOutputImageView() {
	return m_offscreenTarget->getImageView();
}

VkImage MainPassStep::GetOutputImage() {
	return m_offscreenTarget->getImage();
}

void MainPassStep::SetRenderTargetObjects(std::vector<Core::RenderObject>&renderObjects) {
	m_renderObjects = renderObjects;
}


void MainPassStep::createRenderPass() {
	Pipeline::RenderPassConfig config = {
		.colorFormat = m_format,
		.depthFormat = std::make_optional(m_depthImageBuffer->getFormat()),
		.samples = m_sampleCountFlag,
		.multisamplingEnabled = m_sampleCountFlag != VK_SAMPLE_COUNT_1_BIT,
		.outputLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
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
		.vertexShaderPath = "mesh.vert.spv",
		.fragmentShaderPath = "mesh.frag.spv",
		.pipelineLayout = m_pipelineLayout,
		.msaaSamples = m_sampleCountFlag,
		.vertexInputBinding = std::make_optional(VulkanVertexLayout::GetBindingDescription()),
		.vertexInputAttributes = std::make_optional(VulkanVertexLayout::GetAttributeDescriptions()),
		.depthTestEnable = false,
		.depthWriteEnable = false,
		.depthFormat = m_depthImageBuffer->getFormat(),

	};

	m_pipeline = std::make_unique<Pipeline::VulkanPipeline>(config);
}

void MainPassStep::createFramebuffer() {
	auto depthImageView = std::make_optional(m_depthImageBuffer->getImageView());
	m_offscreenTarget = std::make_unique<Pipeline::VulkanOffscreenTarget>(
		m_device, m_physicalDevice, m_extent, m_format, m_renderPass->get(), depthImageView, m_sampleCountFlag);
}

void MainPassStep::createDepthImage() {
	m_depthImageBuffer = std::make_unique<Pipeline::VulkanDepthBuffer>(m_device, m_physicalDevice, m_extent,
	                                                                   m_sampleCountFlag, true);
}
