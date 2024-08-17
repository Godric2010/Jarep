//
// Created by sebastian on 30.11.23.
//

#pragma once

#if defined(__linux__) || defined(_WIN32)
	#include "../../../include/NativeWindowHandleProvider.hpp"
	#include <stdexcept>
	#include <vulkan/vulkan_core.h>

	#if defined(__linux__)
		#define VK_USE_PLATFORM_XLIB_KHR

		#include <vulkan/vulkan_xlib.h>

		#define VK_USE_PLATFORM_WAYLAND_KHR

		#include <vulkan/vulkan_wayland.h>

	#elif defined(_WIN32)
		#define VK_USE_PLATFORM_WIN32_KHR
		#include <vulkan/vulkan_win32.h>
		#include <windows.h>
	#endif


class VulkanSurfaceAdapter {

	public:
	VulkanSurfaceAdapter() = default;

	~VulkanSurfaceAdapter() = default;

	static VkSurfaceKHR CreateSurfaceFromNativeHandle(NativeWindowHandleProvider* nativeWindowHandleProvider, VkInstance instance);

	private:
	static VkSurfaceKHR CreateX11Surface(NativeWindowHandleProvider* nativeWindowHandleProvider, VkInstance instance);

	static VkSurfaceKHR CreateWaylandSurface(NativeWindowHandleProvider* nativeWindowHandleProvider, VkInstance instance);

	static VkSurfaceKHR CreateWindowsSurface(NativeWindowHandleProvider* nativeWindowHandleProvider, VkInstance instance);
};
#endif
