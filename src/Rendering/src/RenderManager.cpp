//
// Created by sebastian on 1/18/2025.
//

#include "RenderManager.hpp"

#include <iostream>

#include "core/VulkanCommandPool.hpp"

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
	createPipeline();
	createFramebuffers();

	// Allocate resources
	createSyncObjects();
	allocateCommandBuffers();

	m_currentFrame = 0;
	m_acquiredImageIndex = 0;

	return true;
}

void RenderManager::Resize(uint32_t width, uint32_t height) {
	vkDeviceWaitIdle(m_core->getDevice()->getDevice());
	m_swapchain->recreate(width, height);
	m_renderPass.reset();
	m_pipeline.reset();
	m_framebuffers.clear();

	createRenderPass();
	createPipeline();
	createFramebuffers();
}

void RenderManager::DrawFrame() {
	beginFrame();

	uint32_t imageIndex = m_acquiredImageIndex;
	VkCommandBuffer cmdBuffer = m_commandBuffers[m_acquiredImageIndex];

	recordCommandBuffer(cmdBuffer, m_acquiredImageIndex);

	endFrame(imageIndex);
}


void RenderManager::Shutdown() {
	// Free resources
	vkFreeCommandBuffers(m_core->getDevice()->getDevice(), m_core->getCommandPool()->get(),
	                     static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
	destroySyncObjects();

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
		// .depthTestEnable = false,
		// .depthWriteEnable = false,
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

void RenderManager::createSyncObjects() {
	uint32_t imageCount = m_swapchain->getImageCount();
	m_imageAvailableSemaphores.resize(imageCount);
	m_renderFinishedSemaphores.resize(imageCount);
	m_inFlightFences.resize(imageCount);
	m_commandBuffers.resize(imageCount);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_core->getDevice()->getDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i])
		    != VK_SUCCESS ||
		    vkCreateSemaphore(m_core->getDevice()->getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i])
		    != VK_SUCCESS ||
		    vkCreateFence(m_core->getDevice()->getDevice(), &fenceInfo, nullptr, &m_inFlightFences[i])
		    != VK_SUCCESS
		) {
			throw std::runtime_error("failed to create fence!");
		}
	}
}

void RenderManager::allocateCommandBuffers() {
	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
	allocateInfo.commandPool = m_core->getCommandPool()->get();

	if (vkAllocateCommandBuffers(m_core->getDevice()->getDevice(), &allocateInfo, m_commandBuffers.data()) !=
	    VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}


void RenderManager::destroySyncObjects() {
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE) {
			vkDestroySemaphore(m_core->getDevice()->getDevice(), m_renderFinishedSemaphores[i], nullptr);
		}
		if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
			vkDestroySemaphore(m_core->getDevice()->getDevice(), m_renderFinishedSemaphores[i], nullptr);
		}
		if (m_inFlightFences[i] != VK_NULL_HANDLE) {
			vkDestroyFence(m_core->getDevice()->getDevice(), m_inFlightFences[i], nullptr);
		}
		m_imageAvailableSemaphores.clear();
		m_renderFinishedSemaphores.clear();
		m_inFlightFences.clear();
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

	std::cout << "Swapchain extent {W: " << m_swapchain->getExtent().width << " H:" << m_swapchain->getExtent().height
			<< "}" << std::endl;

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
	std::cout <<"Drawing triangle for image index: "<< imageIndex << std::endl;

	vkCmdEndRenderPass(cmdBuffer);

	if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void RenderManager::beginFrame() {
	vkWaitForFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_acquiredImageIndex], VK_TRUE, UINT64_MAX);
	vkResetFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_acquiredImageIndex]);

	vkResetCommandBuffer(m_commandBuffers[m_acquiredImageIndex], 0);
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
	vkResetFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_currentFrame]);

	VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signalSemaphore[] = {m_renderFinishedSemaphores[m_currentFrame]};

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_acquiredImageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphore;

	if (vkQueueSubmit(m_core->getDevice()->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_acquiredImageIndex]) !=
	    VK_SUCCESS) {
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

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
