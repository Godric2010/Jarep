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


void RenderManager::Shutdown() {
	// Free resources

	// Destroy render passes and pipelines
	m_framebuffers.clear();
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
