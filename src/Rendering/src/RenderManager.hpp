//
// Created by sebastian on 1/18/2025.
//
#pragma once

#include "core/VulkanCore.hpp"
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

        void initSwapchain(uint32_t width, uint32_t height);
    };
}
