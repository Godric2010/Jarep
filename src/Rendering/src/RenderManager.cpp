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
