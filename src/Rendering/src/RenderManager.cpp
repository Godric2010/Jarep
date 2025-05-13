//
// Created by sebastian on 1/18/2025.
//

#include "RenderManager.hpp"

#include <iostream>

#include "core/VulkanCommandPool.hpp"
#include "steps/FinalBlitStep.hpp"
#include "steps/MainPassStep.hpp"

using namespace JAREP::Rendering;

RenderManager::RenderManager() = default;

RenderManager::~RenderManager() = default;

bool RenderManager::Initialize(const RenderSettings renderSettings, Core::CameraConfig cameraConfig) {
	// Initialize Core systems
	m_core = std::make_unique<Core::VulkanCore>();
	m_core->Initialize(renderSettings);
	clampAndSetSampleCount(renderSettings.msaa);

	m_meshRegistry = std::make_unique<Meshes::VulkanMeshRegistry>(m_core->getDevice()->getDevice(),
	                                                              m_core->getDevice()->getPhysicalDevice());

	m_renderResolution = {renderSettings.renderWidth, renderSettings.renderHeight};
	m_renderStepManager = std::make_unique<Steps::RenderStepManager>();

	// Initialize swapchain
	m_windowResolution = VkExtent2D(renderSettings.renderWidth, renderSettings.renderHeight);
	initSwapchain(renderSettings.width, renderSettings.height);

	// Initialize render steps
	createRenderSteps(cameraConfig);

	// Allocate resources
	createSyncObjects();
	allocateCommandBuffers();

	m_currentFrame = 0;
	m_acquiredImageIndex = 0;


	return true;
}

void RenderManager::Resize(uint32_t width, uint32_t height) {
	vkDeviceWaitIdle(m_core->getDevice()->getDevice());
	destroySyncObjects();

	m_windowResolution = VkExtent2D(width, height);
	m_swapchain->recreate(width, height);
	m_renderStepManager->Resize({width, height});

	createSyncObjects();
	allocateCommandBuffers();
	m_currentFrame = 0;
}

void RenderManager::SetRenderResolution(uint32_t resX, uint32_t resY) {
	m_renderResolution = {resX, resY};
	m_renderStepManager->SetRenderResolution(m_renderResolution);
}

void RenderManager::AddRenderObject(const JAREP::Core::MeshID meshID,
                                    const std::shared_ptr<JAREP::Core::Types::Mesh> mesh,
                                    const InstanceData objectData) {
	if (m_meshRegistry->HasMesh(meshID) == false) {
		m_meshRegistry->AddMesh(mesh, meshID);
	}

	const auto renderObjects = std::vector{
		Core::RenderObject(meshID, objectData),
	};
	m_renderStepManager->SetRenderObjects(renderObjects);
}

void RenderManager::UpdateRenderObject(const Core::RenderObject renderObject) {
	m_renderStepManager->SetRenderObjects(std::vector{renderObject});
}

void RenderManager::DrawFrame() {
	beginFrame();

	uint32_t imageIndex = m_acquiredImageIndex;
	VkCommandBuffer cmdBuffer = m_commandBuffers[m_currentFrame];

	m_renderStepManager->Execute(cmdBuffer);

	endFrame(imageIndex);
}


void RenderManager::Shutdown() {
	m_meshRegistry.reset();

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

void RenderManager::clampAndSetSampleCount(uint8_t requestedSample) {
	bool sampleIsValid = requestedSample >= 1 && requestedSample <= 64 && (requestedSample & (requestedSample - 1)) ==
	                     0;
	VkSampleCountFlagBits maxSampleCount = m_core->getDevice()->getMaxSampleCount();
	if (!sampleIsValid) {
		m_sampleCountFlag = maxSampleCount;
		std::cout << "Invalid sample count. Switched to max value: " << maxSampleCount << std::endl;
		return;
	}

	uint32_t desired = static_cast<uint32_t>(requestedSample);
	uint32_t max = static_cast<uint32_t>(maxSampleCount);
	if (desired <= max) {
		m_sampleCountFlag = static_cast<VkSampleCountFlagBits>(desired);
	}
	else {
		m_sampleCountFlag = maxSampleCount;
	}
	std::cout << "Sample count: " << m_sampleCountFlag << std::endl;
}


void RenderManager::initSwapchain(const uint32_t width, const uint32_t height) {
	const auto vulkanDevice = m_core->getDevice();
	const auto vulkanSurface = m_core->getSurface();

	Pipeline::SwapchainConfig swapchainConfig{};
	swapchainConfig.width = width;
	swapchainConfig.height = height;
	swapchainConfig.vsync = false;

	m_swapchain = std::make_unique<Pipeline::VulkanSwapchain>(vulkanDevice->getDevice(),
	                                                          vulkanDevice->getPhysicalDevice(),
	                                                          vulkanSurface->get(),
	                                                          swapchainConfig);
}

void RenderManager::createRenderSteps(CameraConfig cameraConfig) const {
	auto mainStep = std::make_unique<Steps::MainPassStep>(m_core->getDevice()->getDevice(),
	                                                      m_core->getDevice()->getPhysicalDevice(), m_sampleCountFlag,
	                                                      m_meshRegistry.get(), cameraConfig);
	mainStep->Prepare(m_renderResolution, m_swapchain->getFormat());


	std::function<uint32_t()> getIndexFunc = [&] { return m_acquiredImageIndex; };
	auto&vulkanSwapchain = *m_swapchain;
	auto finalBlit = std::make_unique<Steps::FinalBlitStep>(m_core->getDevice()->getDevice(),
	                                                        m_core->getDevice()->getPhysicalDevice(),
	                                                        std::ref(vulkanSwapchain),
	                                                        getIndexFunc);
	finalBlit->Prepare(m_swapchain->getExtent(), m_swapchain->getFormat());
	finalBlit->BindToOutputOf(mainStep.get());

	m_renderStepManager->AddStep(std::move(mainStep));
	m_renderStepManager->AddStep(std::move(finalBlit));
}


void RenderManager::createSyncObjects() {
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

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
			vkDestroySemaphore(m_core->getDevice()->getDevice(), m_imageAvailableSemaphores[i], nullptr);
		}
		if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
			vkDestroySemaphore(m_core->getDevice()->getDevice(), m_renderFinishedSemaphores[i], nullptr);
		}
		if (m_inFlightFences[i] != VK_NULL_HANDLE) {
			vkDestroyFence(m_core->getDevice()->getDevice(), m_inFlightFences[i], nullptr);
		}
	}
	m_imageAvailableSemaphores.clear();
	m_renderFinishedSemaphores.clear();
	m_inFlightFences.clear();
}

void RenderManager::beginFrame() {
	vkWaitForFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_currentFrame]);

	vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
	VkResult result = vkAcquireNextImageKHR(m_core->getDevice()->getDevice(),
	                                        m_swapchain->get(),
	                                        100000000,
	                                        m_imageAvailableSemaphores[m_currentFrame],
	                                        VK_NULL_HANDLE,
	                                        &m_acquiredImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		std::cout << "Swapchain error! Recreate!" << std::endl;
		Resize(m_windowResolution.width, m_windowResolution.height);
	}
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
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphore;

	VkResult result = vkQueueSubmit(m_core->getDevice()->getGraphicsQueue(), 1, &submitInfo,
	                                m_inFlightFences[m_currentFrame]);
	if (result != VK_SUCCESS) {
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

	VkResult queuePresentResult = vkQueuePresentKHR(m_core->getDevice()->getPresentQueue(), &presentInfo);
	if (queuePresentResult != VK_SUCCESS) {
		throw std::runtime_error("failed to present!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
