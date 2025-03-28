//
// Created by sebastian on 1/18/2025.
//
#pragma once

#include "Rendering/IRenderer.hpp"
#include "core/VulkanCore.hpp"
#include "rendering/VulkanSwapchain.hpp"

namespace JAREP::Rendering {
    class RenderManager : public IRenderer {
    public:
        RenderManager();

        ~RenderManager() override;

        bool Initialize(RenderSettings render_settings) override;

        void Resize(uint32_t width, uint32_t height) override;

        void Shutdown() override;

    private:
        std::unique_ptr<Core::VulkanCore> m_core;
        std::unique_ptr<Rendering::VulkanSwapchain> m_swapchain;

        void initSwapchain(uint32_t width, uint32_t height);
    };
}
