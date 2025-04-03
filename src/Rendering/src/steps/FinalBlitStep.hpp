//
// Created by Godri on 3/30/2025.
//

#pragma once
#include "IRenderStep.hpp"
#include "../pipeline/VulkanRenderPass.hpp"
#include "../pipeline/VulkanPipeline.hpp"

namespace JAREP::Rendering::Steps {
    class FinalBlitStep : public IRenderStep {
    public:
        FinalBlitStep(VkDevice device, VkPhysicalDevice physicalDevice);

        ~FinalBlitStep() override;

        FinalBlitStep(const FinalBlitStep &) = delete;

        FinalBlitStep &operator=(const FinalBlitStep &) = delete;

        void Prepare(VkExtent2D extent, VkFormat format) override;

        void BindToOutputOf(IRenderStep *previousRenderStep) override;

        void Resize(VkExtent2D newExtent) override;

        void Record(VkCommandBuffer cmd) override;

        VkImageView GetOutput() override;

        void SetTargetFramebuffer(VkFramebuffer framebuffer);

    private:

        void createSampler();
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void allocateDescriptorSet();
        void createRenderPass();
        void createPipeline();

        VkDevice m_device;
        VkPhysicalDevice m_physicalDevice;
        VkExtent2D m_extent;
        VkFormat m_format;
        VkPipelineLayout m_pipelineLayout;
        VkFramebuffer m_framebuffer;

        VkDescriptorSetLayout m_descriptorSetLayout;
        VkDescriptorPool m_descriptorPool;
        VkDescriptorSet m_descriptorSet;

        VkSampler m_sampler;

        IRenderStep* m_previousRenderStep;

        std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
        std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;
    };
}
