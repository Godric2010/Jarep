//
// Created by sebastian on 1/18/2025.
//
#pragma once

#include "Rendering/IRenderer.hpp"
#include "core/VulkanCore.hpp"

namespace JAREP::Rendering {
    class RenderManager : public IRenderer {
    public:
        RenderManager();

        ~RenderManager() override;

        bool Initialize(RenderSettings render_settings) override;

        void Shutdown() override;

    private:
        std::unique_ptr<Core::VulkanCore> m_core;
    };
}
