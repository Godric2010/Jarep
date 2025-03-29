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
	uint32_t graphicsQueueIndex = m_device->getGraphicsQueueFamilyIndex().value();
	m_commandPool = std::make_unique<VulkanCommandPool>(m_device->getDevice(), graphicsQueueIndex);

	return true;
}

void VulkanCore::Shutdown() {
	m_commandPool.reset();
	m_device.reset();
	m_surface.reset();
	m_instance->Destroy();
}

const VulkanDevice* VulkanCore::getDevice() const {
	return m_device.get();
}

const VulkanSurface* VulkanCore::getSurface() const {
	return m_surface.get();
}

const VulkanCommandPool* VulkanCore::getCommandPool() const {
	return m_commandPool.get();
}

