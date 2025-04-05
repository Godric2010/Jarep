//
// Created by Godri on 3/30/2025.
//

#pragma once
#include <functional>

#include "IRenderStep.hpp"
#include "../pipeline/VulkanRenderPass.hpp"
#include "../pipeline/VulkanPipeline.hpp"

namespace JAREP::Rendering::Pipeline {
    class VulkanFramebuffer;
}

namespace JAREP::Rendering::Steps {
    class FinalBlitStep : public IRenderStep {
    public:
        FinalBlitStep(VkDevice device, VkPhysicalDevice physicalDevice,
                      const std::vector<VkImageView> &swapchainImageViews,
                      std::function<uint32_t()> getFramebufferIndex);

        ~FinalBlitStep() override;

        FinalBlitStep(const FinalBlitStep &) = delete;

        FinalBlitStep &operator=(const FinalBlitStep &) = delete;

        void Prepare(VkExtent2D extent, VkFormat format, VkQueue graphicsQueue, VkCommandPool commandPool) override;

        void BindToOutputOf(IRenderStep *previousRenderStep) override;

        void Resize(VkExtent2D newExtent) override;

        void Record(VkCommandBuffer cmd) override;

        VkImageView GetOutputImageView() override;

        VkImage GetOutputImage() override;

    private:
        void createSampler();

        void createDescriptorSetLayout();

        void createDescriptorPool();

        void allocateDescriptorSet();

        void createRenderPass();

        void createPipeline();

        void createFramebuffers();

        VkDevice m_device;
        VkPhysicalDevice m_physicalDevice;
        VkExtent2D m_extent;
        VkFormat m_format;
        VkPipelineLayout m_pipelineLayout;
        std::vector<VkImageView> m_swapchainImageViews;

        VkDescriptorSetLayout m_descriptorSetLayout;
        VkDescriptorPool m_descriptorPool;
        VkDescriptorSet m_descriptorSet;

        VkCommandPool m_commandPool;
        VkQueue m_graphicsQueue;

        VkSampler m_sampler;

        IRenderStep *m_previousRenderStep;

        std::function<uint32_t()> m_getFramebufferIndex;

        std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
        std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;
        std::vector<std::unique_ptr<Pipeline::VulkanFramebuffer>> m_framebuffers;
    };
}
