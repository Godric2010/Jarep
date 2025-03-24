//
// Created by sebastian on 3/22/2025.
//

#include "VulkanSurface.hpp"
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanSurface::VulkanSurface(VkInstance instance, RenderSettings render_settings) {
	m_instance = instance;
	m_surface = VK_NULL_HANDLE;

	switch (render_settings.systemType) {
		case SystemType::Windows: {
			createSurfaceWin(render_settings.handle, render_settings.display);
			break;
		}
		case SystemType::X11: {
			createSurfaceX11(render_settings.handle, render_settings.display);
			break;
		}
		default: { break; }
	}
}

VulkanSurface::~VulkanSurface() {
	if (m_surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}
}

VkSurfaceKHR VulkanSurface::get() const {
	return m_surface;
}

void VulkanSurface::createSurfaceWin(void* handle, void* display) {
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(display);
	surfaceCreateInfo.hwnd = static_cast<HWND>(handle);

	if (vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface)) {
		throw std::runtime_error("Failed to create Vulkan surface!");
	}
}

void VulkanSurface::createSurfaceX11(void* handle, void* display) {
}
