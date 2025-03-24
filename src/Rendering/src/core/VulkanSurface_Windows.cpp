//
// Created by Godri on 3/24/2025.
//

#if defined(_WIN32)

#include "VulkanSurface.hpp"
#include <windows.h>
#include <vulkan/vulkan_win32.h>

using namespace JAREP::Rendering::Core;

void VulkanSurface::createSurface(void* handle, void* display) {
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(display);
	surfaceCreateInfo.hwnd = static_cast<HWND>(handle);

	if (vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

#endif
