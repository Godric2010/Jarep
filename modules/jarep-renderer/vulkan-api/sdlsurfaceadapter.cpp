//
// Created by sebastian on 30.11.23.
//


#include "sdlsurfaceadapter.hpp"

namespace Graphics::Vulkan {

	VkSurfaceKHR VulkanSurfaceAdapter::CreateSurfaceFromNativeHandle(
			Graphics::NativeWindowHandleProvider *nativeWindowHandleProvider, VkInstance instance) {

		switch (nativeWindowHandleProvider->getWindowSystem()) {

			case Cocoa:
				throw std::runtime_error("Using a cocoa window with vulkan is not allowed!");
			case Win32: {
				return VulkanSurfaceAdapter::CreateWindowsSurface(nativeWindowHandleProvider, instance);
			}
			case Wayland: {
				return VulkanSurfaceAdapter::CreateWaylandSurface(nativeWindowHandleProvider, instance);
			}
			case X11: {
				return VulkanSurfaceAdapter::CreateX11Surface(nativeWindowHandleProvider, instance);
			}
			default:
				throw std::runtime_error("Invalid window system.");
		}
	}

	VkSurfaceKHR
	VulkanSurfaceAdapter::CreateWindowsSurface(Graphics::NativeWindowHandleProvider *nativeWindowHandleProvider,
	                                           VkInstance instance) {
		VkSurfaceKHR surface;
#if defined(_WIN32)
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = static_cast<HWND>(nativeWindowHandleProvider->getNativeWindowHandle());
		createInfo.hinstance = GetModuleHandle(nullptr);

		if(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface)) {
			throw std::runtime_error("Failed to create Win32 m_surface");
		}
#endif
		return surface;
	}

	VkSurfaceKHR VulkanSurfaceAdapter::CreateX11Surface(
			Graphics::NativeWindowHandleProvider *nativeWindowHandleProvider, VkInstance instance) {
		VkSurfaceKHR surface;
#if defined(__linux__) || defined(__unix__)
		auto xcbWindowHandle = dynamic_cast<XlibWindowHandleProvider *>(nativeWindowHandleProvider);

		VkXlibSurfaceCreateInfoKHR xlibCreateInfo = {};
		xlibCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		xlibCreateInfo.dpy = xcbWindowHandle->getDisplayConnection();
		xlibCreateInfo.window = xcbWindowHandle->getXcbWindow();

		if (vkCreateXlibSurfaceKHR(instance, &xlibCreateInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create XCB Surface!");
		}
#endif
		return surface;
	}

	VkSurfaceKHR VulkanSurfaceAdapter::CreateWaylandSurface(
			Graphics::NativeWindowHandleProvider *nativeWindowHandleProvider, VkInstance instance) {
		VkSurfaceKHR surface;
#if defined(__linux__) || defined(__unix__)
		auto waylandWindowHandle = dynamic_cast<WaylandWindowHandleProvider *>(nativeWindowHandleProvider);

		VkWaylandSurfaceCreateInfoKHR waylandCreateInfo = {};
		waylandCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
		waylandCreateInfo.display = waylandWindowHandle->getDisplay();
		waylandCreateInfo.surface = waylandWindowHandle->getWindowHandle();

		if (vkCreateWaylandSurfaceKHR(instance, &waylandCreateInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Wayland m_surface.");
		}
#endif
		return surface;
	}
}
