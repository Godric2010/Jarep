//
// Created by sebastian on 1/18/2025.
//

#include "VulkanCore.hpp"
using namespace JAREP::Rendering::Core;

VulkanCore::VulkanCore() = default;

VulkanCore::~VulkanCore() = default;

bool VulkanCore::Initialize(RenderSettings render_settings) {
    m_instance = std::make_unique<VulkanInstance>(render_settings.extensions);
    m_surface = std::make_unique<VulkanSurface>(m_instance->getInstance(), render_settings);
    m_device = std::make_unique<VulkanDevice>(m_instance->getInstance(), m_surface->get());

    return true;
}

void VulkanCore::Shutdown() {
    m_device.reset();
    m_surface.reset();
    m_instance->Destroy();
}
