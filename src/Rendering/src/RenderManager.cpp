//
// Created by sebastian on 1/18/2025.
//

#include "RenderManager.hpp"

using namespace JAREP::Rendering;

RenderManager::RenderManager() = default;

RenderManager::~RenderManager() = default;

bool RenderManager::Initialize(RenderSettings render_settings) {
	// Initialize Core systems
	m_core = std::make_unique<Core::VulkanCore>();
	m_core->Initialize(render_settings);

	// Initialize swapchain
	initSwapchain(render_settings.width, render_settings.height);

	// Initialize Pipelines and Render passes
	createRenderPass();
	createFramebuffers();

	// Allocate resources

	return true;
}

void RenderManager::Resize(uint32_t width, uint32_t height) {
	vkDeviceWaitIdle(m_core->getDevice()->getDevice());
	m_swapchain->recreate(width, height);
}

void RenderManager::DrawFrame() {
	beginFrame();

	uint32_t imageIndex = m_acquiredImageIndex;
	VkCommandBuffer cmdBuffer = m_commandBuffers[imageIndex];

	recordCommandBuffer(cmdBuffer, imageIndex);

	endFrame(imageIndex);
}


void RenderManager::Shutdown() {
	// Free resources

	// Destroy render passes and pipelines
	m_framebuffers.clear();
	m_pipeline.reset();
	vkDestroyPipelineLayout(m_core->getDevice()->getDevice(), m_pipelineLayout, nullptr);
	m_renderPass.reset();

	// Destroy swapchain
	m_swapchain.reset();

	// Destroy core elements
	m_core->Shutdown();
}

void RenderManager::initSwapchain(uint32_t width, uint32_t height) {
	auto vulkanDevice = m_core->getDevice();
	auto vulkanSurface = m_core->getSurface();

	Pipeline::SwapchainConfig swapchainConfig{};
	swapchainConfig.width = width;
	swapchainConfig.height = height;

	m_swapchain = std::make_unique<Pipeline::VulkanSwapchain>(vulkanDevice->getDevice(),
	                                                          vulkanDevice->getPhysicalDevice(),
	                                                          vulkanSurface->get(),
	                                                          swapchainConfig);
}

void RenderManager::createRenderPass() {
	Pipeline::RenderPassConfig config = {
		.colorFormat = m_swapchain->getFormat(),
		.depthFormat = std::nullopt,
		.samples = VK_SAMPLE_COUNT_1_BIT,
	};

	m_renderPass = std::make_unique<Pipeline::VulkanRenderPass>(m_core->getDevice()->getDevice(), config);
}


void RenderManager::createPipeline() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_core->getDevice()->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
	    VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	Pipeline::VulkanPipelineConfig config = {
		.device = m_core->getDevice()->getDevice(),
		.renderPass = m_renderPass->get(),
		.extent = m_swapchain->getExtent(),
		.vertexShaderPath = "triangle.vert.spv",
		.fragmentShaderPath = "triangle.frag.spv",
		.pipelineLayout = m_pipelineLayout,
		.depthFormat = std::nullopt,
	};

	m_pipeline = std::make_unique<Pipeline::VulkanPipeline>(config);
}


void RenderManager::createFramebuffers() {
	m_framebuffers.clear();

	const auto&imageViews = m_swapchain->getImageViews();
	std::optional<VkImageView> depthView = std::nullopt;
	for (auto&imageView: imageViews) {
		std::vector<VkImageView> attachments = {imageView};
		if (depthView.has_value()) {
			attachments.push_back(depthView.value());
		}

		m_framebuffers.push_back(std::make_unique<Pipeline::VulkanFramebuffer>(
			m_core->getDevice()->getDevice(),
			m_renderPass->get(),
			m_swapchain->getExtent(),
			attachments));
	}
}

void RenderManager::recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->get();
	renderPassInfo.framebuffer = m_framebuffers[imageIndex]->get();
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = m_swapchain->getExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapchain->getExtent().width);
	viewport.height = static_cast<float>(m_swapchain->getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.extent = m_swapchain->getExtent();
	scissor.offset = {0, 0};
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());
	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(cmdBuffer);

	if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void RenderManager::beginFrame() {
	vkWaitForFences(m_core->getDevice()->getDevice(),1,&m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(m_core->getDevice()->getDevice(),
		m_swapchain->get(),
		UINT64_MAX,
		m_imageAvailableSemaphores[m_currentFrame],
		VK_NULL_HANDLE,
		&m_acquiredImageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire image from swapchain!");
	}
}

void RenderManager::endFrame(uint32_t imageIndex) {
	vkResetFences(m_core->getDevice()->getDevice(),1,&m_inFlightFences[m_currentFrame]);

	VkSemaphore waitSemphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signalSemaphore[] = {m_imageAvailableSemaphores[m_currentFrame]};

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphore;

	if (vkQueueSubmit(m_core->getDevice()->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame])!= VK_SUCCESS) {
		throw std::runtime_error("failed to submit command buffer command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphore;

	VkSwapchainKHR swapchains[] = {m_swapchain->get()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(m_core->getDevice()->getPresentQueue(), &presentInfo);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present!");
	}

	m_currentFrame = (m_currentFrame + 1) % 3;

}

