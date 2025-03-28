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

	// Destroy swapchain
	m_swapchain.reset();

	// Destroy core elements
	m_core->Shutdown();
}

void RenderManager::initSwapchain(uint32_t width, uint32_t height) {
	auto vulkanDevice = m_core->getDevice();
	auto vulkanSurface = m_core->getSurface();

	Rendering::SwapchainConfig swapchainConfig{};
	swapchainConfig.width = width;
	swapchainConfig.height = height;

	m_swapchain = std::make_unique<Rendering::VulkanSwapchain>(vulkanDevice->getDevice(),
	                                                           vulkanDevice->getPhysicalDevice(),
	                                                           vulkanSurface->get(),
	                                                           swapchainConfig);
}
