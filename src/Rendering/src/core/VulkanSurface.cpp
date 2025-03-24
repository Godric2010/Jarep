//
// Created by sebastian on 3/22/2025.
//

#include "VulkanSurface.hpp"

using namespace JAREP::Rendering::Core;

VulkanSurface::VulkanSurface(VkInstance instance, RenderSettings render_settings) {
	m_instance = instance;
	m_surface = VK_NULL_HANDLE;

	createSurface(render_settings.handle, render_settings.display);
}

VulkanSurface::~VulkanSurface() {
	if (m_surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}
}

VkSurfaceKHR VulkanSurface::get() const {
	return m_surface;
}
