//
// Created by sebastian on 1/18/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "Rendering/IRenderer.hpp"

namespace JAREP::Rendering::Core {
    class VulkanCore {
    public:
        VulkanCore();

        ~VulkanCore();

        bool Initialize(RenderSettings render_settings);

        void Shutdown();

    private:
        std::unique_ptr<VulkanInstance> m_instance;
        std::unique_ptr<VulkanSurface> m_surface;
    };
}
