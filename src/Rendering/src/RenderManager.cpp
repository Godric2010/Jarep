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

bool RenderManager::Initialize(RenderSettings render_settings) {
    // Initialize Core systems
    m_core = std::make_unique<Core::VulkanCore>();
    m_core->Initialize(render_settings);

    m_renderResolution = {render_settings.renderWidth, render_settings.renderHeight};
    m_renderStepManager = std::make_unique<Steps::RenderStepManager>();

    // Initialize swapchain
    initSwapchain(render_settings.width, render_settings.height);

    // Initialize render steps
    createOneTimeSubmitPool();
    createRenderSteps();

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
    m_renderStepManager->Resize({width, height});
}

void RenderManager::SetRenderResolution(uint32_t resX, uint32_t resY) {
    m_renderResolution = {resX, resY};
    m_renderStepManager->SetRenderResolution(m_renderResolution);
}

void RenderManager::DrawFrame() {
    beginFrame();

    uint32_t imageIndex = m_acquiredImageIndex;
    VkCommandBuffer cmdBuffer = m_commandBuffers[m_acquiredImageIndex];

    // recordCommandBuffer(cmdBuffer, m_acquiredImageIndex);
    m_renderStepManager->Execute(cmdBuffer);

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

void RenderManager::createRenderSteps() {
    auto mainStep = std::make_unique<Steps::MainPassStep>(m_core->getDevice()->getDevice(),
                                                          m_core->getDevice()->getPhysicalDevice());
    mainStep->Prepare(m_renderResolution, m_swapchain->getFormat(), m_core->getDevice()->getGraphicsQueue(),
                      m_oneTimeSubmitPool);


    std::function<uint32_t()> getIndexFunc = [&] { return m_acquiredImageIndex; };
    auto finalBlit = std::make_unique<Steps::FinalBlitStep>(m_core->getDevice()->getDevice(),
                                                            m_core->getDevice()->getPhysicalDevice(),
                                                            m_swapchain->getImageViews(),
                                                            getIndexFunc);
    finalBlit->Prepare(m_swapchain->getExtent(), m_swapchain->getFormat(), m_core->getDevice()->getGraphicsQueue(),
                       m_oneTimeSubmitPool);
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
            vkDestroySemaphore(m_core->getDevice()->getDevice(), m_renderFinishedSemaphores[i], nullptr);
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

void RenderManager::createOneTimeSubmitPool() {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_core->getDevice()->getGraphicsQueueFamilyIndex().value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(m_core->getDevice()->getDevice(), &poolInfo, nullptr, &m_oneTimeSubmitPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create one time submit pool!");
    }
}


void RenderManager::beginFrame() {
    vkWaitForFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(m_core->getDevice()->getDevice(), 1, &m_inFlightFences[m_currentFrame]);

    vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
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

    if (vkQueueSubmit(m_core->getDevice()->getGraphicsQueue(), 1, &submitInfo,
                      m_inFlightFences[m_acquiredImageIndex]) !=
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
