//
// Created by sebastian on 1/18/2025.
//
#pragma once

#include "core/VulkanCore.hpp"
#include "pipeline/VulkanFramebuffer.hpp"
#include "pipeline/VulkanPipeline.hpp"
#include "pipeline/VulkanRenderPass.hpp"
#include "pipeline/VulkanSwapchain.hpp"
#include "Rendering/IRenderer.hpp"

namespace JAREP::Rendering {
    struct RenderSettings;

    class RenderManager : public IRenderer {
    public:
        RenderManager();

        ~RenderManager() override;

        bool Initialize(RenderSettings render_settings) override;

        void Resize(uint32_t width, uint32_t height) override;

        void Shutdown() override;

    private:
        std::unique_ptr<Core::VulkanCore> m_core;
        std::unique_ptr<Pipeline::VulkanSwapchain> m_swapchain;

        std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
        std::vector<std::unique_ptr<Pipeline::VulkanFramebuffer>> m_framebuffers;
        std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout;


        void initSwapchain(uint32_t width, uint32_t height);
        void createRenderPass();
        void createFramebuffers();
        void createPipeline();
    };
}
