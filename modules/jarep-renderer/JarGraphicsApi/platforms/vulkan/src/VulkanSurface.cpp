//
// Created by sebastian on 16.08.24.
//

#include "../include/VulkanSurface.hpp"

VulkanSurface::VulkanSurface(std::shared_ptr<JarBackend>& backend, NativeWindowHandleProvider* windowHandleProvider) {

	auto vulkanBackend = reinterpret_cast<std::shared_ptr<VulkanBackend>&>(backend);

	m_surfaceExtent = VkExtent2D();
	m_surfaceExtent.width = windowHandleProvider->GetWindowWidth();
	m_surfaceExtent.height = windowHandleProvider->GetWindowHeight();

	m_surface = VulkanSurfaceAdapter::CreateSurfaceFromNativeHandle(windowHandleProvider, vulkanBackend->GetVulkanInstance());
}

VulkanSurface::~VulkanSurface() { Release(); }

void VulkanSurface::Release() { std::cout << "Release vulkan surface" << std::endl; }

void VulkanSurface::RecreateSurface(JarExtent2D extent) {
	m_surfaceExtent.height = extent.height;
	m_surfaceExtent.width = extent.width;

	std::cout << "Recreate surface" << std::endl;
}

uint32_t VulkanSurface::GetSwapchainImageAmount() { return 0; }

JarExtent2D VulkanSurface::GetSurfaceExtent() {
	JarExtent2D extent2D{};
	extent2D.width = m_surfaceExtent.width;
	extent2D.height = m_surfaceExtent.height;
	return extent2D;
}