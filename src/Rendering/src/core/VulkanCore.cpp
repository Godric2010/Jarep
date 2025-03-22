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
    return true;
}

void VulkanCore::Shutdown() {
    m_instance->Destroy();
}
