//
// Created by Godri on 3/30/2025.
//

#include "FinalBlitStep.hpp"

#include <filesystem>
#include <utility>

#include "../core/VulkanCore.hpp"
#include "../core/VulkanImageOperations.hpp"
#include "../pipeline/VulkanFramebuffer.hpp"

using namespace JAREP::Rendering::Steps;

FinalBlitStep::FinalBlitStep(VkDevice device, VkPhysicalDevice physicalDevice,
                             const std::vector<VkImageView> &swapchainImageViews,
                             std::function<uint32_t()> getFramebufferIndex) {
    m_device = device;
    m_physicalDevice = physicalDevice;
    m_getFramebufferIndex = std::move(getFramebufferIndex);
    m_swapchainImageViews = swapchainImageViews;
    m_extent = VkExtent2D();
    m_pipelineLayout = VK_NULL_HANDLE;
    m_framebuffers = std::vector<std::unique_ptr<Pipeline::VulkanFramebuffer> >();
    m_renderPass = VK_NULL_HANDLE;
    m_pipeline = VK_NULL_HANDLE;
    m_previousRenderStep = nullptr;
    m_descriptorPool = VK_NULL_HANDLE;
    m_descriptorSet = VK_NULL_HANDLE;
    m_descriptorSetLayout = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;
    m_format = VK_FORMAT_B8G8R8A8_SRGB;
}

FinalBlitStep::~FinalBlitStep() {
    m_pipeline.reset();
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    }

    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
    }
    if (m_descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
    }
    if (m_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(m_device, m_sampler, nullptr);
    }

    m_renderPass.reset();
}

void FinalBlitStep::Prepare(VkExtent2D extent, VkFormat format, VkQueue graphicsQueue, VkCommandPool commandPool) {
    m_extent = extent;
    m_format = format;
    m_graphicsQueue = graphicsQueue;
    m_commandPool = commandPool;

    createSampler();
    createDescriptorSetLayout();
    createDescriptorPool();
    allocateDescriptorSet();
    createRenderPass();
    createFramebuffers();
    createPipeline();
}

void FinalBlitStep::BindToOutputOf(IRenderStep *previousRenderStep) {
    m_previousRenderStep = previousRenderStep;

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = previousRenderStep->GetOutputImageView();
    imageInfo.sampler = m_sampler;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
}

void FinalBlitStep::Resize(VkExtent2D newExtent) {
    m_extent = newExtent;
    m_pipeline.reset();
    m_framebuffers.clear();
    m_renderPass.reset();

    createRenderPass();
    createFramebuffers();
    createPipeline();
}

void FinalBlitStep::Record(VkCommandBuffer cmd) {
    uint32_t index = m_getFramebufferIndex();



    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass->get();
    renderPassInfo.framebuffer = m_framebuffers[index]->get();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_extent;

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_extent.width);
    viewport.height = static_cast<float>(m_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = m_extent;
    scissor.offset = {0, 0};
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());
    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd);
}

VkImageView FinalBlitStep::GetOutputImageView() {
    return VK_NULL_HANDLE;
}

VkImage FinalBlitStep::GetOutputImage() {
    return VK_NULL_HANDLE;
}

void FinalBlitStep::createSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create sampler!");
    }
}

void FinalBlitStep::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void FinalBlitStep::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;


    if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void FinalBlitStep::allocateDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    if (vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}


void FinalBlitStep::createRenderPass() {
    Pipeline::RenderPassConfig config = {
        .colorFormat = m_format,
        .depthFormat = std::nullopt,
        .samples = VK_SAMPLE_COUNT_1_BIT,
    };

    m_renderPass = std::make_unique<Pipeline::VulkanRenderPass>(m_device, config);
}

void FinalBlitStep::createPipeline() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
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
        .vertexShaderPath = "fullscreen.vert.spv",
        .fragmentShaderPath = "blit.frag.spv",
        .pipelineLayout = m_pipelineLayout,
        .depthFormat = std::nullopt,
        // .depthTestEnable = false,
        // .depthWriteEnable = false,
    };

    m_pipeline = std::make_unique<Pipeline::VulkanPipeline>(config);
}

void FinalBlitStep::createFramebuffers() {
    m_framebuffers.clear();

    std::optional<VkImageView> depthView = std::nullopt;
    for (auto &imageView: m_swapchainImageViews) {
        std::vector<VkImageView> attachments = {imageView};
        if (depthView.has_value()) {
            attachments.push_back(depthView.value());
        }

        m_framebuffers.push_back(std::make_unique<Pipeline::VulkanFramebuffer>(
            m_device,
            m_renderPass->get(),
            m_extent,
            attachments));
    }
}
