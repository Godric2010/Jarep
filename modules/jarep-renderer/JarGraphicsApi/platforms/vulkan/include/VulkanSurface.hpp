//
// Created by sebastian on 16.08.24.
//

#pragma once

#include "../../../include/JarSurface.hpp"
#include "VulkanBackend.hpp"
#include "VulkanSurfaceAdapter.hpp"
#include <vulkan/vulkan_core.h>

class VulkanSurface : public JarSurface {
	public:
	VulkanSurface(std::shared_ptr<JarBackend>& backend, NativeWindowHandleProvider* windowHandleProvider);

	~VulkanSurface() override;

	void RecreateSurface(JarExtent2D extent) override;

	void Release() override;

	uint32_t GetSwapchainImageAmount() override;

	JarExtent2D GetSurfaceExtent() override;

	private:
	VkSurfaceKHR m_surface;
	VkExtent2D m_surfaceExtent{};
};
